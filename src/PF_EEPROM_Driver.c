DEFINE_FAKE_VOID_FUNC2(PF_EEPROM_GetVersion, int, pf_eeprom_version*);

#define DECLARE_ARG(type, n, FUNCNAME) \
    type arg##n##_val; \
    type arg##n##_history[FFF_ARG_HISTORY_LEN];

#define SAVE_ARG(FUNCNAME, n) \
    memcpy((void*)&FUNCNAME##_fake.arg##n##_val, (void*)&arg##n, sizeof(arg##n));

#define DECLARE_FAKE_VOID_FUNC2(FUNCNAME, ARG0_TYPE, ARG1_TYPE) \
    extern "C" \
        typedef struct PF_EEPROM_GetVersion_Fake { \
            int arg_0_val; \
            pf_eeprom_version* arg_0_history[50]; \
            DECLARE_ALL_FUNC_COMMON \
            DECLARE_CUSTOM_FAKE_SEQ_VARIABLES \
            void(*custom_fake)(int arg0, pf_eeprom_version* arg1); \
            void(**custom_fake_seq)(int arg0, pf_eeprom_version* arg1); \
        } PF_EEPROM_GetVersion##_Fake;\
        extern PF_EEPROM_GetVersion##_Fake PF_EEPROM_GetVersion##_fake;\
        void PF_EEPROM_GetVersion_reset(void); \
    } \

#define DEFINE_FAKE_VOID_FUNC2(FUNCNAME/*PF_EEPROM_GetVersion*/, ARG0_TYPE, ARG1_TYPE) \
    extern "C" \
        PF_EEPROM_GetVersion_Fake PF_EEPROM_GetVersion_fake;\
        void PF_EEPROM_GetVersion(int arg0, pf_eeprom_version* arg1){ \
            SAVE_ARG(PF_EEPROM_GetVersion, 0); \
            memcpy((void *)&PF_EEPROM_GetVersion_fake.arg_0_val, (void*)&arg_0)
            SAVE_ARG(PF_EEPROM_GetVersion, 1); \
            if(ROOM_FOR_MORE_HISTORY(PF_EEPROM_GetVersion)){\
                SAVE_ARG_HISTORY(PF_EEPROM_GetVersion, 0); \
                SAVE_ARG_HISTORY(PF_EEPROM_GetVersion, 1); \
            }\
            else{\
                HISTORY_DROPPED(PF_EEPROM_GetVersion);\
            }\
            INCREMENT_CALL_COUNT(PF_EEPROM_GetVersion); \
            REGISTER_CALL(PF_EEPROM_GetVersion); \
            if (PF_EEPROM_GetVersion_fake.custom_fake_seq_len){ /* a sequence of custom fakes */ \
                if (PF_EEPROM_GetVersion_fake.custom_fake_seq_idx < PF_EEPROM_GetVersion_fake.custom_fake_seq_len){ \
                    PF_EEPROM_GetVersion_fake.custom_fake_seq[PF_EEPROM_GetVersion_fake.custom_fake_seq_idx++](arg0, arg1); \
                } \
                else{ \
                    PF_EEPROM_GetVersion_fake.custom_fake_seq[PF_EEPROM_GetVersion_fake.custom_fake_seq_len-1](arg0, arg1); \
                } \
            } \
            if (PF_EEPROM_GetVersion_fake.custom_fake)  PF_EEPROM_GetVersion_fake.custom_fake(arg0, arg1); \
        } \
        DEFINE_RESET_FUNCTION(PF_EEPROM_GetVersion) \
    } \

