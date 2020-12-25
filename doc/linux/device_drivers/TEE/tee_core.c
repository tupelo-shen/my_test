static int __init tee_init(void)
{
    int rc;
//新建一个class，这样就可以在/sys/class下面看到tee了
    tee_class = class_create(THIS_MODULE, "tee");
    if (IS_ERR(tee_class)) {
        pr_err("couldn't create class\n");
        return PTR_ERR(tee_class);
    }
//申请一个tee字符设备的驱动号
    rc = alloc_chrdev_region(&tee_devt, 0, TEE_NUM_DEVICES, "tee");
    if (rc) {
        pr_err("failed to allocate char dev region\n");
        class_destroy(tee_class);
        tee_class = NULL;
    }

    return rc;
}
//系统会自动调用tee_init
subsys_initcall(tee_init);
这样tee_core.c 做的事情就完了
继续看optee/core.c
static int __init optee_driver_init(void)
{
    struct device_node *fw_np;
    struct device_node *np;
    struct optee *optee;

    /* Node is supposed to be below /firmware */
//从device tree中找到firemare的节点，看起来tee 目前只支持device tree还不支持acpi
    fw_np = of_find_node_by_name(NULL, "firmware");
    if (!fw_np)
        return -ENODEV;
//再从firmware 节点中找到匹配的节点，这里匹配的是linaro,optee-tz
    np = of_find_matching_node(fw_np, optee_match);
    of_node_put(fw_np);
    if (!np)
        return -ENODEV;
//调用optee_probe得到一个struct optee *optee;最终把这个值赋值给静态全局变量optee_svc

    optee = optee_probe(np);
    of_node_put(np);

    if (IS_ERR(optee))
        return PTR_ERR(optee);

    optee_svc = optee;

    return 0;
}
作为模块被kenrel调用
module_init(optee_driver_init);
static struct optee *optee_probe(struct device_node *np)
{
    optee_invoke_fn *invoke_fn;
    struct tee_shm_pool *pool;
    struct optee *optee = NULL;
    void *memremaped_shm = NULL;
    struct tee_device *teedev;
    u32 sec_caps;
    int rc;
//获取op-tee驱动是通过hvc还是smc 切换到monitor接口
    invoke_fn = get_invoke_func(np);
    if (IS_ERR(invoke_fn))
        return (void *)invoke_fn;
//通过invoke_fn 陷入到secure world api uid 是否匹配
    if (!optee_msg_api_uid_is_optee_api(invoke_fn)) {
        pr_warn("api uid mismatch\n");
        return ERR_PTR(-EINVAL);
    }
//通过invoke_fn 陷入到secure world api revision 是否匹配
    if (!optee_msg_api_revision_is_compatible(invoke_fn)) {
        pr_warn("api revision mismatch\n");
        return ERR_PTR(-EINVAL);
    }
//从secure world中拿到reserved share memory
    if (!optee_msg_exchange_capabilities(invoke_fn, &sec_caps)) {
        pr_warn("capabilities mismatch\n");
        return ERR_PTR(-EINVAL);
    }

    /*
     * We have no other option for shared memory, if secure world
     * doesn't have any reserved memory we can use we can't continue.
     */
//share memory是secure world和normal world 通信用的，如果secure world没有预留这部分就直接退出
    if (!(sec_caps & OPTEE_SMC_SEC_CAP_HAVE_RESERVED_SHM))
        return ERR_PTR(-EINVAL);
//配置share memory建立地址映射
    pool = optee_config_shm_memremap(invoke_fn, &memremaped_shm);
    if (IS_ERR(pool))
        return (void *)pool;
//为struct optee 申请内存
    optee = kzalloc(sizeof(*optee), GFP_KERNEL);
    if (!optee) {
        rc = -ENOMEM;
        goto err;
    }
//保存normal world陷入到secure world的函数
    optee->invoke_fn = invoke_fn;

    teedev = tee_device_alloc(&optee_desc, NULL, pool, optee);
    if (IS_ERR(teedev)) {
        rc = PTR_ERR(teedev);
        goto err;
    }
    optee->teedev = teedev;
//创建dev/tee 驱动
    teedev = tee_device_alloc(&optee_supp_desc, NULL, pool, optee);
    if (IS_ERR(teedev)) {
        rc = PTR_ERR(teedev);
        goto err;
    }
    optee->supp_teedev = teedev;
//注册和libteec通信的字符设备
    rc = tee_device_register(optee->teedev);
    if (rc)
        goto err;
//注册和tee_supplicant通信的字符设备
    rc = tee_device_register(optee->supp_teedev);
    if (rc)
        goto err;

    mutex_init(&optee->call_queue.mutex);
    INIT_LIST_HEAD(&optee->call_queue.waiters);
    optee_wait_queue_init(&optee->wait_queue);
    optee_supp_init(&optee->supp);
    optee->memremaped_shm = memremaped_shm;
    optee->pool = pool;
//使能共享内存
    optee_enable_shm_cache(optee);

    pr_info("initialized driver\n");
    return optee;
err:
    if (optee) {
        /*
         * tee_device_unregister() is safe to call even if the
         * devices hasn't been registered with
         * tee_device_register() yet.
         */
        tee_device_unregister(optee->supp_teedev);
        tee_device_unregister(optee->teedev);
        kfree(optee);
    }
    if (pool)
        tee_shm_pool_free(pool);
    if (memremaped_shm)
        memunmap(memremaped_shm);
    return ERR_PTR(rc);
}
通过get_invoke_func 获取和monitor通信的接口
static optee_invoke_fn *get_invoke_func(struct device_node *np)
{
    const char *method;

    pr_info("probing for conduit method from DT.\n");

    if (of_property_read_string(np, "method", &method)) {
        pr_warn("missing \"method\" property\n");
        return ERR_PTR(-ENXIO);
    }
//根据device tree中的method来判断是通过hvc还是smc，一般是通过smc
    if (!strcmp("hvc", method))
        return optee_smccc_hvc;
    else if (!strcmp("smc", method))
        return optee_smccc_smc;

    pr_warn("invalid \"method\" property: %s\n", method);
    return ERR_PTR(-EINVAL);
}

static void optee_smccc_smc(unsigned long a0, unsigned long a1,
                unsigned long a2, unsigned long a3,
                unsigned long a4, unsigned long a5,
                unsigned long a6, unsigned long a7,
                struct arm_smccc_res *res)
{
    arm_smccc_smc(a0, a1, a2, a3, a4, a5, a6, a7, res);
}
#define arm_smccc_smc(...) __arm_smccc_smc(__VA_ARGS__, NULL)
其定义在arch/arm64/kernel/smccc-call.S
ENTRY(__arm_smccc_smc)
     SMCCC   smc
 ENDPROC(__arm_smccc_smc)
这个函数用户判断secure world是否预留memory来和normal world共享信息
static bool optee_msg_exchange_capabilities(optee_invoke_fn *invoke_fn,
                        u32 *sec_caps)
{
    union {
        struct arm_smccc_res smccc;
        struct optee_smc_exchange_capabilities_result result;
    } res;
    u32 a1 = 0;

    /*
     * TODO This isn't enough to tell if it's UP system (from kernel
     * point of view) or not, is_smp() returns the the information
     * needed, but can't be called directly from here.
     */
    if (!IS_ENABLED(CONFIG_SMP) || nr_cpu_ids == 1)
        a1 |= OPTEE_SMC_NSEC_CAP_UNIPROCESSOR;
//通过invoke_fn 陷入到monitor，其结果保存在res.smccc，这里注意res是一个union。
    invoke_fn(OPTEE_SMC_EXCHANGE_CAPABILITIES, a1, 0, 0, 0, 0, 0, 0,
          &res.smccc);
//先判断是否成功返回
    if (res.result.status != OPTEE_SMC_RETURN_OK)
        return false;
//保存monitor返回的信息，后面在从sec_caps 中判断是否已经预留的共享内存
    *sec_caps = res.result.capabilities;
    return true;
}