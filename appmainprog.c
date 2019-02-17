/***************************************************************
LEGAL DISCLAIMER
(Header of MediaTek Software/Firmware Release or Documentation)
Mediatek Inc (C) 2017~2018. All right reserved.

File name: appmainprog.c

Author:

Description:

others:
************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "u_common.h"
#include "u_amb.h"
#include "u_am.h"
#include "u_assert.h"
#include "u_cli.h"
#include "u_dbg.h"
#include "u_app_thread.h"
#include "u_c4a_stub.h"
#include "u_assistant_stub.h"
#include "u_ga_stub.h"
#include "u_app_def.h"
#include "u_acfg.h"
#include "mas_lib.h"
#include "led.h"
#include "u_ipcd.h"

#include "Mtkc4a_ota_update_server.h"
#include "ri_c4a_ota_update_common.h"
#include "_rpc_ipc_util.h"

#include "dlist.h"
#include "easyipc_base.h"
#include "tl_utils_acfg.h"
#include "tonly_script.h"

/*-----------------------------------------------------------------------------
 * macro definitions
 *---------------------------------------------------------------------------*/
#define APPMAIN_TAG "<appmain>"
#define APPMAIN_DEBUG(x) printf x
#define APPMAIN_OK 0
#define APPMAIN_FAIL -1

#define GVA_AVS_SCRIPT_FILE_PATH    "/etc/gva_avs_config/script" //add by laim

UINT32 gUi4EnableAllLog = 1;

ipc_handle *pt_eipc_send_handle = NULL;

/*-----------------------------------------------------------------------------
 * structure definitions
 *---------------------------------------------------------------------------*/
typedef struct _APP_MNGR_INIT_SEQ_T
{
    HANDLE_T h_sema;/* Semaphore handle for the application manager sequencing. */
    BOOL b_ok;/* Boolean state indicating application manager init successfull. */
}APP_MNGR_INIT_SEQ_T;


typedef VOID (*app_register_fct)(AMB_REGISTER_INFO_T * );


typedef struct{
    app_register_fct _regfct;
    AMB_REGISTER_INFO_T _reginfo;
    char * app_name;
}APP_REG;


/*-----------------------------------------------------------------------------
 * extern function declarations
 *---------------------------------------------------------------------------*/
extern VOID a_hellotest_register(AMB_REGISTER_INFO_T* pt_reg);
#if CONFIG_SUPPORT_DM_APP
extern VOID a_dm_register(AMB_REGISTER_INFO_T* pt_reg);
#endif
extern VOID a_sm_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID aPlaybackUriRegister(AMB_REGISTER_INFO_T* ptReg);
extern VOID aPlaybackTtsRegister(AMB_REGISTER_INFO_T* ptReg);
extern VOID aPlaybackPromptRegister(AMB_REGISTER_INFO_T* ptReg);
extern VOID aPlaybackLineInRegister(AMB_REGISTER_INFO_T* ptReg);
extern VOID a_playback_usb_register(AMB_REGISTER_INFO_T* ptReg);
extern VOID a_button_sound_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID a_timerd_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID a_acfg_register(AMB_REGISTER_INFO_T*  pt_reg);
extern VOID aTdmInRegister(AMB_REGISTER_INFO_T*  ptReg);
extern VOID aUacCaptureRegister(AMB_REGISTER_INFO_T*  ptReg);
extern VOID uiAppUserInterfaceRegister(AMB_REGISTER_INFO_T*  pReg);
#if CONFIG_SUPPORT_BT_APP
extern VOID aBluetoothRegister(AMB_REGISTER_INFO_T* pt_reg);
#endif
#if CONFIG_SUPPORT_DLNA_APP
extern VOID aPlaybackDlnaRegister(AMB_REGISTER_INFO_T* ptReg);
#endif
extern VOID a_wifi_setting_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID a_misc_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID aUpgCtrlRegister(AMB_REGISTER_INFO_T* pt_reg);
#if CONFIG_SUPPORT_BT_HFP_APP
extern VOID aBluetoothHfpRegister(AMB_REGISTER_INFO_T* pt_reg);
#endif
extern VOID a_led_manager_register(AMB_REGISTER_INFO_T* pt_reg);
extern VOID aAvsStubRegister(AMB_REGISTER_INFO_T* ptReg);
#if CONFIG_SUPPORT_BLE_MESH_APP
extern VOID aBleMeshAppRegister(AMB_REGISTER_INFO_T* pt_reg);
#endif  /* CONFIG_SUPPORT_BLE_MESH_APP */


/*-----------------------------------------------------------------------------
 * private function declarations
 *---------------------------------------------------------------------------*/
VOID xAppInit (VOID);


/*-----------------------------------------------------------------------------
 * variable declarations
 *---------------------------------------------------------------------------*/
void* gMasHandle = NULL;

static APP_REG appToReg[] = {
        {a_led_manager_register,{0},LED_MANAGER_THREAD_NAME},
        {a_timerd_register,{0},TIMERD_THREAD_NAME},
        {a_acfg_register,{0},ACFG_THREAD_NAME},
#ifdef VA_SUPPORT_AVS
		{aAvsStubRegister,{0},AVS_STUB_THREAD_NAME},
#endif
        {a_sm_register,{0},SM_THREAD_NAME},
        {a_wifi_setting_register,{0},WIFI_SETTING_THREAD_NAME},
        {a_misc_register,{0},MISC_THREAD_NAME},
		{aUpgCtrlRegister,{0},UPG_CONTROL_THREAD_NAME},
#if CONFIG_SUPPORT_DM_APP
		{a_dm_register,{0},DM_THREAD_NAME},
#endif
		{uiAppUserInterfaceRegister,{0},USER_INTERFACE_THREAD_NAME},
#if CONFIG_SUPPORT_BT_APP
		//{aBluetoothRegister,{0},BLUETOOTH_THREAD_NAME},
#endif

#ifndef SUPPORT_GVA_STUB
#ifndef SUPPORT_GA_STUB
        {aPlaybackUriRegister,{0},PB_URI_THREAD_NAME},
        {aPlaybackTtsRegister,{0},PB_TTS_THREAD_NAME},
        {aPlaybackPromptRegister,{0},PB_PROMPT_THREAD_NAME},
        {aPlaybackLineInRegister,{0},PB_LINE_IN_THREAD_NAME},
        {a_playback_usb_register,{0},PB_USB_THREAD_NAME},
        {aTdmInRegister,{0},PB_TDM_IN_THREAD_NAME},
        {aUacCaptureRegister,{0},PB_UAC_CAPTURE_THREAD_NAME},
#if CONFIG_SUPPORT_DLNA_APP
        {aPlaybackDlnaRegister,{0},PB_DLNA_THREAD_NAME},
#endif

#if CONFIG_SUPPORT_BT_HFP_APP
        {aBluetoothHfpRegister,{0},BLUETOOTH_HFP_THREAD_NAME},
#endif

#if CONFIG_SUPPORT_BUTTON_SOUND_APP
        {a_button_sound_register,{0},BTN_SOUND_THREAD_NAME},
#endif
		{aAssistantStubRegister,{0},ASSISTANT_STUB_THREAD_NAME},
#endif
#endif

#ifdef VA_SUPPORT_AVS
		{aAvsStubRegister,{0},AVS_STUB_THREAD_NAME},
#endif

#ifdef SUPPORT_GVA_STUB
        {a_c4a_stub_register,{0},C4A_STUB_THREAD_NAME},
#endif

#ifdef SUPPORT_GA_STUB
		{aGaStubRegister,{0},GA_STUB_THREAD_NAME},
#endif

#if CONFIG_SUPPORT_BLE_MESH_APP
        {aBleMeshAppRegister,{0},BLE_MESH_THREAD_NAME},
#endif /* CONFIG_SUPPORT_BLE_MESH_APP */

        {NULL,{0},NULL}
};

/*************add by tonly**************/
int eipc_init()
{
    pt_eipc_send_handle = ipc_creat("appmainprog");

    printf("<appmain>appmainprog eipc init!!!!\n");

    if (NULL == pt_eipc_send_handle)
    {
        printf("ipc_creat appmainprog failed!\n");
        return FALSE;
    }

    return TRUE;
}

/*************add end*******************/

/*---------------------------------------------------------------------------
 * Name
 *      appMngrNfyFct
 * Description      -
 * Input arguments  -
 * Output arguments -
 * Returns          -
 *---------------------------------------------------------------------------*/
static BOOL appMngrNfyFct (HANDLE_T          h_app_mngr,
                              VOID*             pv_tag,
                              APP_NFY_REASON_T  e_nfy_reason)
{
    if ((pv_tag != NULL)&&
    ((e_nfy_reason == APP_NFY_INIT_OK)||
    (e_nfy_reason == APP_NFY_INIT_FAILED)))
    {
        if (e_nfy_reason == APP_NFY_INIT_OK)
        {
            ((APP_MNGR_INIT_SEQ_T*) pv_tag)->b_ok = TRUE;
        }

        ASSERT(uSemaUnlock (((APP_MNGR_INIT_SEQ_T*) pv_tag)->h_sema) == OSR_OK);
    }

    return (FALSE);
}

/*---------------------------------------------------------------------------
 * Name
 *      xAppInit
 * Description      -
 * Input arguments  -
 * Output arguments -
 * Returns          -
 *---------------------------------------------------------------------------*/
VOID xAppInit (VOID)
{
    HANDLE_T              h_app_mngr;
    APP_MNGR_INIT_SEQ_T   t_app_mngr_init_seq;
    AMB_REGISTER_INFO_T   t_amb_reg_info;

    /* Create the application manager synchronization semaphore. */
    ASSERT(uSemaCreate(&t_app_mngr_init_seq.h_sema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);


    /* Initialize application manager */
    u_am_init (& t_amb_reg_info);


    t_app_mngr_init_seq.b_ok = FALSE;
    /* Start application manager */
    ASSERT (u_app_start (&(t_amb_reg_info.t_fct_tbl),
                     &(t_amb_reg_info.t_desc),
                     t_amb_reg_info.s_name,
                     appMngrNfyFct,
                     ((VOID*) &t_app_mngr_init_seq),
                     &h_app_mngr) == AEER_OK);

    /* And now wait until the application manager has signaled that it */
    /* has successfully started.                                       */
    ASSERT(uSemaLock (t_app_mngr_init_seq.h_sema, X_SEMA_OPTION_WAIT) == OSR_OK);
    /* If the application manager could not start successfully, abort.*/
    ASSERT ((t_app_mngr_init_seq.b_ok));
    /* Free the application manager sequencing semaphore. */
    ASSERT (uSemaDelete (t_app_mngr_init_seq.h_sema) == OSR_OK);


    return;
}

/*---------------------------------------------------------------------------
 * Name
 *      app main
 * Description      -
 * Input arguments  -
 * Output arguments -
 * Returns          -
 *---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    pid_t pid;
    int ret = APPMAIN_OK;
    int status = 0;
    int i = 0;
    APP_REG * app = appToReg;

    eipc_init(); //add by laim, init eipc

    if (NULL != pt_eipc_send_handle)
    {
        //init tl script
        tonly_script_init(pt_eipc_send_handle, GVA_AVS_SCRIPT_FILE_PATH);
        //init tl acfg
        tl_utils_acfg_client_init(pt_eipc_send_handle);
    }

    /*coredump function*/
    if(0 == access("/data/enable_coredump", 0))
    {
        ipcd_exec("ulimit -c 200000", NULL);
        ipcd_exec("mkdir /data/coredump", NULL);
        ipcd_exec("echo /data/coredump/core.%e.%p.%t > /proc/sys/kernel/core_pattern", NULL);
    }
    else
    {
        ipcd_exec("ulimit -c 0", NULL);
        ipcd_exec("echo /data/coredump/core.%e.%p.%t > /proc/sys/kernel/core_pattern", NULL);
    }

#if CONFIG_SUPPORT_APPMAIN_RESTART
    restart:

    pid = fork();

    if(0 > pid)
    {
        APPMAIN_DEBUG((APPMAIN_TAG"fork child process error at line %d\n",__LINE__));
    }
    else if(0 < pid)
    {
        /*parent process*/
        APPMAIN_DEBUG((APPMAIN_TAG"parent process pid is %d, child process pid is %d\n",getpid(),pid));
        int quit_pid = waitpid(pid,&status,0);
        APPMAIN_DEBUG((APPMAIN_TAG"quit pid is %d, exit status is %d\n",quit_pid,WEXITSTATUS(status)));

        APPMAIN_DEBUG((APPMAIN_TAG"kill btservice\n"));
        /*kill main btservice process*/
        ipcd_exec("killall -9 btservice", NULL);

        /*need to remove the alsa device handle used flag*/
        ipcd_exec("rm -f /tmp/mtkac0_opened", NULL);
        ipcd_exec("rm -f /tmp/mtkac1_opened", NULL);
        ipcd_exec("rm -f /tmp/mtkac2_opened", NULL);

        /*restart bt service*/
        ipcd_exec("/usr/bin/btservice&", NULL);
        goto restart;
    }
    else
#endif
    {
        /*init output log type*/
        if(0 == access("/data/log_all",0))
        {
            //gUi4EnableAllLog = 0;
            //APPMAIN_DEBUG((APPMAIN_TAG"disable all ouput in appmainprog!!\n"));
        }
        APPMAIN_DEBUG((APPMAIN_TAG"**********************************************\n"));
        APPMAIN_DEBUG((APPMAIN_TAG"child process id is %d\n",getpid()));
        APPMAIN_DEBUG((APPMAIN_TAG"Appcliation Init Begin\n"));
        APPMAIN_DEBUG((APPMAIN_TAG"Audio Mas process Init\n"));

#ifndef SUPPORT_GVA_STUB
        CHECK_ASSERT(mas_create(&gMasHandle));
#endif
        //I2S 1 playback
        //ipcd_exec("amixer -c 0 cset name='O03 I05 Switch' 1", NULL);//tonly delete
        //ipcd_exec("amixer -c 0 cset name='O04 I06 Switch' 1", NULL);//tonly delete
        //ipcd_exec("amixer -c 0 cset name='I2S O03_O04 Switch' 1", NULL);//tonly delete

#ifndef SUPPORT_GVA_STUB
        //select hw:0,6 to playback
        mas_device_config_ext_t config;
        mas_chmap_desc_t hw_spk;
        mas_chmap_desc_t usr_spk;
        char *outputaddr = "plug:dmix";//tonly hw:0,6--> plug:dmix

        memset(&config,0,sizeof(config));
        hw_spk.channels = 2;
        hw_spk.chmap[0] = MAS_CHMAP_FL;
        hw_spk.chmap[1] = MAS_CHMAP_FR;
        usr_spk.channels = 2;
        usr_spk.chmap[0] = MAS_CHMAP_FL;
        usr_spk.chmap[1] = MAS_CHMAP_FR;
        config.hw_spk_chmap = &hw_spk;
        config.user_spk_chmap = &usr_spk;
        config.bits = 32;
        config.addr = outputaddr;
        ret = mas_set_device_desc_ext(gMasHandle, MT_AUDIO_OUT_SPEAKER, &config);
        if(ret != 0)
        {
            APPMAIN_DEBUG((APPMAIN_TAG"%s,%d,error!\n",__FUNCTION__,__LINE__));
        }
#ifdef  SUPPORT_GA_STUB
        APPMAIN_DEBUG((APPMAIN_TAG"GASDK set amixer at line %d\n",__LINE__));
        ipcd_exec("amixer  cset name='Left PGA Mux' 2", NULL);
        ipcd_exec("amixer  cset name='Right PGA Mux' 2", NULL);
        ipcd_exec("amixer  cset name='O09 I03 Switch' on", NULL);
        ipcd_exec("amixer  cset name='O10 I04 Switch' on", NULL);
        ipcd_exec("amixer  cset name='AIN Mux' 0", NULL);
        ipcd_exec("amixer  cset name='AIF TX Mux' 1", NULL);
		ipcd_exec("/usr/bin/mdnsd&",NULL);
#endif

#if 0
        //if custom's SPK only support 48k, please set fix sample rate to MAS
        CHECK_ASSERT(mas_set_out_samplerate_rule(gMasHandle,MAS_OUT_SR_FIX,48000));
        CHECK_ASSERT(mas_set_src_type(gMasHandle,MAS_BLISRC_SRC));
        CHECK_ASSERT(mas_ppc_enable(gMasHandle,1,1));
#endif
		/* add by alwin */
		CHECK_ASSERT(mas_ppc_enable(gMasHandle, MAS_PPC_MIXER_OUT, 1));
		/* end alwin add */
        CHECK_ASSERT(mas_start(gMasHandle));
#endif
        APPMAIN_DEBUG((APPMAIN_TAG"OS thread Init\n"));
        CHECK_ASSERT(osThreadInit());

#if CLI_SUPPORT
        APPMAIN_DEBUG((APPMAIN_TAG"Cli Init\n"));
        CHECK_ASSERT(u_cli_init());
#endif

        APPMAIN_DEBUG((APPMAIN_TAG"Dbg Backtrace Init\n"));
        CHECK_ASSERT(dbgInit());

        APPMAIN_DEBUG((APPMAIN_TAG"AEE Init\n"));
        CHECK_ASSERT(aee_init());

        APPMAIN_DEBUG((APPMAIN_TAG"Handle Usr Init\n"));
        CHECK_ASSERT(uHandleUsrInit(256));

        APPMAIN_DEBUG((APPMAIN_TAG"Am Init\n"));
        xAppInit();
#ifdef SUPPORT_GVA_STUB
		APPMAIN_DEBUG((APPMAIN_TAG"[YF]OTA Update IPCRPC server start\n"));
		c_rpc_init_mtkc4a_ota_update_server();
		rpcu_tl_log_start();
		c_rpc_start_c4a_ota_update_server();
		printf("[YF]OTA Update IPCRPC server end\n");
#endif
        while(app->_regfct != NULL)
        {
            APPMAIN_DEBUG((APPMAIN_TAG"%d.%s Init Begin\n",i+1,app->app_name));
            app->_regfct(&app->_reginfo);
            if(APPMAIN_OK == (ret = u_amb_register_app(&app->_reginfo)))
            {
                if(APPMAIN_OK == (ret = u_amb_sync_start_app(app->app_name)))
                    APPMAIN_DEBUG((APPMAIN_TAG"%d.%s Init Successful\n",i+1,app->app_name));
                else
                    APPMAIN_DEBUG((APPMAIN_TAG"%d.%s sync start fail ret %d\n",i+1,app->app_name,ret));
            }
            else
            {
                APPMAIN_DEBUG((APPMAIN_TAG"%d.%s register fail ret %d\n",i+1,app->app_name,ret));
            }
            app = &appToReg[++i];
        }
        APPMAIN_DEBUG((APPMAIN_TAG"Appcliation Init Finish\n"));
        APPMAIN_DEBUG((APPMAIN_TAG"**********************************************\n"));

#ifdef SUPPORT_GA_STUB
        APPMAIN_DEBUG((APPMAIN_TAG"Turn off led light!\n"));
        fdLedInit();
#endif
        while(1)
        {
            sleep(1);
        }
    }
}
