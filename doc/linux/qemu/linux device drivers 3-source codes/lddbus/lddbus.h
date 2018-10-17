/*
 * 虚拟ldd总线使用的定义。
 */
extern struct bus_type ldd_bus_type;

/*
 * ldd驱动类型
 */
struct ldd_driver {
    char *version;
    struct module *module;
    struct device_driver driver;
    struct driver_attribute version_attr;
};

#define to_ldd_driver(drv) container_of(drv, struct ldd_driver, driver);

/*
 * 插入ldd总线的设备类型
 */
struct ldd_device {
    char *name;
    struct ldd_driver *driver;
    struct device dev;
};

#define to_ldd_device(dev) container_of(dev, struct ldd_device, dev);

/*
 * 对外提供的ldd总线设备注册接口
 */
extern int register_ldd_device(struct ldd_device *);
extern void unregister_ldd_device(struct ldd_device *);
extern int register_ldd_driver(struct ldd_driver *);
extern void unregister_ldd_driver(struct ldd_driver *);