/******************************************************************************
*
* Copyright (C) Chaoyong Zhou
* Email: bgnvendor@163.com
* QQ: 2796796
*
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif/*__cplusplus*/

#include "type.h"
#include "mm.h"
#include "log.h"

#include "creg.h"

#include "tbd.h"
#include "crun.h"
#include "super.h"

#include "cmpie.h"
#include "cstring.h"

#include "kbuff.h"
#include "cdfs.h"
#include "cdfsnp.h"
#include "cdfsdn.h"
#include "csocket.h"
#include "real.h"
#include "cload.h"
#include "cbgt.h"
#include "csession.h"
#include "crfsdn.h"
#include "crfs.h"
#include "crfsc.h"
#include "crfsmon.h"
#include "cxfsdn.h"
#include "cxfs.h"
#include "cxfsmon.h"
#include "chfs.h"
#include "chfsmon.h"
#include "csfs.h"
#include "csfsmon.h"
#include "ctdns.h"
#include "cdetect.h"
#include "cdetectn.h"
#include "ctdnssv.h"
#include "cp2p.h"
#include "cfile.h"

#include "cmd5.h"
#include "cbuffer.h"
#include "tasks.h"
#include "cstrkv.h"
#include "chttp.h"

#include "findex.inc"

#include "tbd.inc"
#include "crun.inc"
#include "super.inc"

#include "cdfs.inc"
#include "cbgt.inc"
#include "csession.inc"
#include "crfs.inc"
#include "cxfs.inc"
#include "chfs.inc"
#include "csfs.inc"
#include "crfsbk.inc"
#include "crfsc.inc"
#include "crfsmon.inc"
#include "cxfsmon.inc"
#include "chfsmon.inc"
#include "csfsmon.inc"
#include "chttp.inc"
#include "ctdns.inc"
#include "cdetect.inc"
#include "cdetectn.inc"
#include "ctdnssv.inc"
#include "cp2p.inc"
#include "cfile.inc"
#include "task.inc"

TYPE_CONV_ITEM *creg_type_conv_item_new()
{
    TYPE_CONV_ITEM *type_conv_item;

    alloc_static_mem(MM_TYPE_CONV_ITEM, &type_conv_item, LOC_CREG_0001);
    if(NULL_PTR == type_conv_item)
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_type_conv_item_new: new type conv item failed\n");
        return (NULL_PTR);
    }
    creg_type_conv_item_init(type_conv_item);
    return (type_conv_item);
}

EC_BOOL creg_type_conv_item_init(TYPE_CONV_ITEM *type_conv_item)
{
    TYPE_CONV_ITEM_VAR_DBG_TYPE(type_conv_item)     = e_dbg_type_end;
    TYPE_CONV_ITEM_VAR_SIZEOF(type_conv_item)       = 0;
    TYPE_CONV_ITEM_VAR_POINTER_FLAG(type_conv_item) = EC_FALSE;
    TYPE_CONV_ITEM_VAR_MM_TYPE(type_conv_item)      = MM_END;
    TYPE_CONV_ITEM_VAR_INIT_FUNC(type_conv_item)    = 0;
    TYPE_CONV_ITEM_VAR_CLEAN_FUNC(type_conv_item)   = 0;
    TYPE_CONV_ITEM_VAR_FREE_FUNC(type_conv_item)    = 0;
    TYPE_CONV_ITEM_VAR_ENCODE_FUNC(type_conv_item)  = 0;
    TYPE_CONV_ITEM_VAR_DECODE_FUNC(type_conv_item)  = 0;
    TYPE_CONV_ITEM_VAR_ENCODE_SIZE(type_conv_item)  = 0;

    return (EC_TRUE);
}

EC_BOOL creg_type_conv_item_clean(TYPE_CONV_ITEM *type_conv_item)
{
    TYPE_CONV_ITEM_VAR_DBG_TYPE(type_conv_item)     = e_dbg_type_end;
    TYPE_CONV_ITEM_VAR_SIZEOF(type_conv_item)       = 0;
    TYPE_CONV_ITEM_VAR_POINTER_FLAG(type_conv_item) = EC_FALSE;
    TYPE_CONV_ITEM_VAR_MM_TYPE(type_conv_item)      = MM_END;
    TYPE_CONV_ITEM_VAR_INIT_FUNC(type_conv_item)    = 0;
    TYPE_CONV_ITEM_VAR_CLEAN_FUNC(type_conv_item)   = 0;
    TYPE_CONV_ITEM_VAR_FREE_FUNC(type_conv_item)    = 0;
    TYPE_CONV_ITEM_VAR_ENCODE_FUNC(type_conv_item)  = 0;
    TYPE_CONV_ITEM_VAR_DECODE_FUNC(type_conv_item)  = 0;
    TYPE_CONV_ITEM_VAR_ENCODE_SIZE(type_conv_item)  = 0;
    return (EC_TRUE);
}

EC_BOOL creg_type_conv_item_free(TYPE_CONV_ITEM *type_conv_item)
{
    if(NULL_PTR != type_conv_item)
    {
        creg_type_conv_item_clean(type_conv_item);
        free_static_mem(MM_TYPE_CONV_ITEM, type_conv_item, LOC_CREG_0002);
    }
    return (EC_TRUE);
}

CVECTOR *creg_type_conv_vec_fetch()
{
    TASK_BRD *task_brd;
    task_brd = task_brd_default_get();
    if(NULL_PTR == task_brd)
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_type_conv_vec_fetch: task_brd not init\n");
        return (NULL_PTR);
    }
    return TASK_BRD_TYPE_CONV_VEC(task_brd);
}

EC_BOOL creg_type_conv_vec_init(CVECTOR *type_conv_vec)
{
    cvector_init(type_conv_vec, CREG_TYPE_CONV_ITEM_DEFAULT_NUM, MM_TYPE_CONV_ITEM, CVECTOR_LOCK_ENABLE, LOC_CREG_0003);
    return (EC_TRUE);
}

EC_BOOL creg_type_conv_vec_clean(CVECTOR *type_conv_vec)
{
    cvector_clean(type_conv_vec, (CVECTOR_DATA_CLEANER)creg_type_conv_item_free, LOC_CREG_0004);
    return (EC_TRUE);
}

TYPE_CONV_ITEM *creg_type_conv_vec_get(CVECTOR *type_conv_vec, const UINT32 var_dbg_type)
{
    return (TYPE_CONV_ITEM *)cvector_get(type_conv_vec, var_dbg_type);
}

EC_BOOL creg_type_conv_vec_add(CVECTOR *type_conv_vec,
                                         const UINT32 var_dbg_type, const UINT32 var_sizeof, const UINT32 var_pointer_flag, const UINT32 var_mm_type,
                                         const UINT32 var_init_func, const UINT32 var_clean_func, const UINT32 var_free_func,
                                         const UINT32 var_encode_func, const UINT32 var_decode_func, const UINT32 var_encode_size
                                         )
{
    TYPE_CONV_ITEM *type_conv_item;
    UINT32 pos;

    if(NULL_PTR != cvector_get(type_conv_vec, var_dbg_type))
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_type_conv_vec_add: type conv item for var_dbg_type %ld was already defined\n", var_dbg_type);
        return (EC_FALSE);
    }

    for(pos = cvector_size(type_conv_vec); pos <= var_dbg_type; pos ++)
    {
        cvector_push(type_conv_vec, NULL_PTR);
    }

    type_conv_item = creg_type_conv_item_new();
    if(NULL_PTR == type_conv_item)
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_type_conv_vec_add: new type conv item failed\n");
        return (EC_FALSE);
    }

    TYPE_CONV_ITEM_VAR_DBG_TYPE(type_conv_item)     = var_dbg_type;
    TYPE_CONV_ITEM_VAR_SIZEOF(type_conv_item)       = var_sizeof;
    TYPE_CONV_ITEM_VAR_POINTER_FLAG(type_conv_item) = var_pointer_flag;
    TYPE_CONV_ITEM_VAR_MM_TYPE(type_conv_item)      = var_mm_type;
    TYPE_CONV_ITEM_VAR_INIT_FUNC(type_conv_item)    = var_init_func;
    TYPE_CONV_ITEM_VAR_CLEAN_FUNC(type_conv_item)   = var_clean_func;
    TYPE_CONV_ITEM_VAR_FREE_FUNC(type_conv_item)    = var_free_func;
    TYPE_CONV_ITEM_VAR_ENCODE_FUNC(type_conv_item)  = var_encode_func;
    TYPE_CONV_ITEM_VAR_DECODE_FUNC(type_conv_item)  = var_decode_func;
    TYPE_CONV_ITEM_VAR_ENCODE_SIZE(type_conv_item)  = var_encode_size;

    cvector_set(type_conv_vec, var_dbg_type, (void *)type_conv_item);
    return (EC_TRUE);
}

EC_BOOL creg_type_conv_vec_add_default(CVECTOR *type_conv_vec)
{
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_UINT32,
        /* type_sizeof            */sizeof(UINT32),
        /* pointer_flag           */EC_FALSE,
        /* var_mm_type            */MM_UINT32,
        /* init_type_func         */0,/*(UINT32)dbg_init_uint32_ptr*/
        /* clean_type_func        */0,/*(UINT32)dbg_clean_uint32_ptr*/
        /* free_type_func         */0,/*(UINT32)dbg_free_uint32_ptr*/
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint32,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint32,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint32_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_UINT16,
        /* type_sizeof            */sizeof(UINT16),
        /* pointer_flag           */EC_FALSE,
        /* var_mm_type            */MM_UINT16,
        /* init_type_func         */0,
        /* clean_type_func        */0,
        /* free_type_func         */0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint16,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint16,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint16_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_UINT8,
        /* type_sizeof            */sizeof(UINT8),
        /* pointer_flag           */EC_FALSE,
        /* var_mm_type            */MM_UINT8,
        /* init_type_func         */0,
        /* clean_type_func        */0,
        /* free_type_func         */0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint8,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint8,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint8_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_void,
        /* type_sizeof            */sizeof(UINT32),
        /* pointer_flag           */EC_FALSE,
        /* var_mm_type            */MM_UINT32,
        /* init_type_func         */0,
        /* clean_type_func        */0,
        /* free_type_func         */0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint32,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint32,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint32_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_EC_BOOL,
        /* type_sizeof            */sizeof(EC_BOOL),
        /* pointer_flag           */EC_FALSE,
        /* var_mm_type            */MM_UINT32,
        /* init_type_func         */0,
        /* clean_type_func        */0,
        /* free_type_func         */0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint32,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint32,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint32_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_REAL_ptr,
        /* type_sizeof            */sizeof(REAL *),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_REAL,
        /* init_type_func         */(UINT32)real_init,
        /* clean_type_func        */(UINT32)real_clean,
        /* free_type_func         */(UINT32)real_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_real,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_real,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_real_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_MOD_MGR_ptr,
        /* type_sizeof            */sizeof(MOD_MGR *),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_MOD_MGR,
        /* init_type_func         */(UINT32)mod_mgr_init,
        /* clean_type_func        */(UINT32)mod_mgr_clean,
        /* free_type_func         */(UINT32)mod_mgr_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_mod_mgr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_mod_mgr,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_mod_mgr_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSTRING_ptr,
        /* type_sizeof            */sizeof(CSTRING *),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSTRING,
        /* init_type_func         */(UINT32)cstring_init_0,
        /* clean_type_func        */(UINT32)cstring_clean,
        /* free_type_func         */(UINT32)cstring_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cstring,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cstring,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cstring_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_TASKC_MGR_ptr,
        /* type_sizeof            */sizeof(TASKC_MGR *),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_TASKC_MGR,
        /* init_type_func         */(UINT32)taskc_mgr_init,
        /* clean_type_func        */(UINT32)taskc_mgr_clean,
        /* free_type_func         */(UINT32)taskc_mgr_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_taskc_mgr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_taskc_mgr,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_taskc_mgr_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_UINT32_ptr,
        /* type_sizeof            */sizeof(UINT32),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_UINT32,
        /* init_type_func         */(UINT32)dbg_init_uint32_ptr,
        /* clean_type_func        */(UINT32)dbg_clean_uint32_ptr,
        /* free_type_func         */(UINT32)dbg_free_uint32_ptr,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint32_ptr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint32,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint32_ptr_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_LOG_ptr,
        /* type_sizeof            */sizeof(LOG),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_LOG,
        /* init_type_func         */(UINT32)log_init,
        /* clean_type_func        */(UINT32)log_clean,
        /* free_type_func         */(UINT32)log_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_log,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_log,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_log_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CVECTOR_ptr,
        /* type_sizeof            */sizeof(CVECTOR),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CVECTOR,
        /* init_type_func         */(UINT32)cvector_init_0,
        /* clean_type_func        */(UINT32)cvector_clean_0,
        /* free_type_func         */(UINT32)cvector_free_0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cvector,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cvector,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cvector_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_KBUFF_ptr,
        /* type_sizeof            */sizeof(KBUFF),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_KBUFF,
        /* init_type_func         */(UINT32)kbuff_init_0,
        /* clean_type_func        */(UINT32)kbuff_clean_0,
        /* free_type_func         */(UINT32)kbuff_free_0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_kbuff,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_kbuff,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_kbuff_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSOCKET_CNODE_ptr,
        /* type_sizeof            */sizeof(CSOCKET_CNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSOCKET_CNODE,
        /* init_type_func         */(UINT32)csocket_cnode_init,
        /* clean_type_func        */(UINT32)csocket_cnode_clean,
        /* free_type_func         */(UINT32)csocket_cnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csocket_cnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csocket_cnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csocket_cnode_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_TASKC_NODE_ptr,
        /* type_sizeof            */sizeof(TASKC_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_TASKC_NODE,
        /* init_type_func         */(UINT32)taskc_node_init,
        /* clean_type_func        */(UINT32)taskc_node_clean,
        /* free_type_func         */(UINT32)taskc_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_taskc_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_taskc_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_taskc_node_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSYS_CPU_STAT_ptr,
        /* type_sizeof            */sizeof(CSYS_CPU_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSYS_CPU_STAT,
        /* init_type_func         */(UINT32)csys_cpu_stat_init,
        /* clean_type_func        */(UINT32)csys_cpu_stat_clean,
        /* free_type_func         */(UINT32)csys_cpu_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csys_cpu_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csys_cpu_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csys_cpu_stat_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_MM_MAN_OCCUPY_NODE_ptr,
        /* type_sizeof            */sizeof(MM_MAN_OCCUPY_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_MM_MAN_OCCUPY_NODE,
        /* init_type_func         */(UINT32)mm_man_occupy_node_init,
        /* clean_type_func        */(UINT32)mm_man_occupy_node_clean,
        /* free_type_func         */(UINT32)mm_man_occupy_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_mm_man_occupy_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_mm_man_occupy_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_mm_man_occupy_node_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_MM_MAN_LOAD_NODE_ptr,
        /* type_sizeof            */sizeof(MM_MAN_LOAD_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_MM_MAN_LOAD_NODE,
        /* init_type_func         */(UINT32)mm_man_load_node_init,
        /* clean_type_func        */(UINT32)mm_man_load_node_clean,
        /* free_type_func         */(UINT32)mm_man_load_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_mm_man_load_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_mm_man_load_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_mm_man_load_node_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_MM_MOD_NODE_ptr,
        /* type_sizeof            */sizeof(MM_MOD_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_MOD_NODE,
        /* init_type_func         */(UINT32)mod_node_init,
        /* clean_type_func        */(UINT32)mod_node_clean,
        /* free_type_func         */(UINT32)mod_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_mod_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_mod_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_mod_node_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CPROC_MODULE_STAT_ptr,
        /* type_sizeof            */sizeof(CPROC_MODULE_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CPROC_MODULE_STAT,
        /* init_type_func         */(UINT32)cproc_module_stat_init,
        /* clean_type_func        */(UINT32)cproc_module_stat_clean,
        /* free_type_func         */(UINT32)cproc_module_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cproc_module_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cproc_module_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cproc_module_stat_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRANK_THREAD_STAT_ptr,
        /* type_sizeof            */sizeof(CRANK_THREAD_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRANK_THREAD_STAT,
        /* init_type_func         */(UINT32)crank_thread_stat_init,
        /* clean_type_func        */(UINT32)crank_thread_stat_clean,
        /* free_type_func         */(UINT32)crank_thread_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crank_thread_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crank_thread_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crank_thread_stat_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSYS_ETH_STAT_ptr,
        /* type_sizeof            */sizeof(CSYS_ETH_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSYS_ETH_STAT,
        /* init_type_func         */(UINT32)csys_eth_stat_init,
        /* clean_type_func        */(UINT32)csys_eth_stat_clean,
        /* free_type_func         */(UINT32)csys_eth_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csys_eth_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csys_eth_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csys_eth_stat_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSYS_DSK_STAT_ptr,
        /* type_sizeof            */sizeof(CSYS_DSK_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSYS_DSK_STAT,
        /* init_type_func         */(UINT32)csys_dsk_stat_init,
        /* clean_type_func        */(UINT32)csys_dsk_stat_clean,
        /* free_type_func         */(UINT32)csys_dsk_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csys_dsk_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csys_dsk_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csys_dsk_stat_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_TASK_REPORT_NODE_ptr,
        /* type_sizeof            */sizeof(TASK_REPORT_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_TASK_REPORT_NODE,
        /* init_type_func         */(UINT32)task_report_node_init,
        /* clean_type_func        */(UINT32)task_report_node_clean,
        /* free_type_func         */(UINT32)task_report_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_task_report_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_task_report_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_task_report_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CDFSNP_FNODE_ptr,
        /* type_sizeof            */sizeof(CDFSNP_FNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CDFSNP_FNODE,
        /* init_type_func         */(UINT32)cdfsnp_fnode_init,
        /* clean_type_func        */(UINT32)cdfsnp_fnode_clean,
        /* free_type_func         */(UINT32)cdfsnp_fnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cdfsnp_fnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cdfsnp_fnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cdfsnp_fnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CDFSNP_ITEM_ptr,
        /* type_sizeof            */sizeof(CDFSNP_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CDFSNP_ITEM,
        /* init_type_func         */(UINT32)cdfsnp_item_init,
        /* clean_type_func        */(UINT32)cdfsnp_item_clean,
        /* free_type_func         */(UINT32)cdfsnp_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cdfsnp_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cdfsnp_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cdfsnp_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CDFSDN_STAT_ptr,
        /* type_sizeof            */sizeof(CDFSDN_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CDFSDN_STAT,
        /* init_type_func         */(UINT32)cdfsdn_stat_init,
        /* clean_type_func        */(UINT32)cdfsdn_stat_clean,
        /* free_type_func         */(UINT32)cdfsdn_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cdfsdn_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cdfsdn_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cdfsdn_stat_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CLOAD_STAT_ptr,
        /* type_sizeof            */sizeof(CLOAD_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CLOAD_STAT,
        /* init_type_func         */(UINT32)cload_stat_init,
        /* clean_type_func        */(UINT32)cload_stat_clean,
        /* free_type_func         */(UINT32)cload_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cload_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cload_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cload_stat_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CLOAD_NODE_ptr,
        /* type_sizeof            */sizeof(CLOAD_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CLOAD_NODE,
        /* init_type_func         */(UINT32)cload_node_init_0,
        /* clean_type_func        */(UINT32)cload_node_clean,
        /* free_type_func         */(UINT32)cload_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cload_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cload_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cload_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CLOAD_MGR_ptr,
        /* type_sizeof            */sizeof(CLOAD_MGR),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CLIST,
        /* init_type_func         */(UINT32)cload_mgr_init,
        /* clean_type_func        */(UINT32)cload_mgr_clean,
        /* free_type_func         */(UINT32)cload_mgr_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cload_mgr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cload_mgr,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cload_mgr_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CDFSDN_RECORD_ptr,
        /* type_sizeof            */sizeof(CDFSDN_RECORD),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CDFSDN_RECORD,
        /* init_type_func         */(UINT32)cdfsdn_record_init,
        /* clean_type_func        */(UINT32)cdfsdn_record_clean,
        /* free_type_func         */(UINT32)cdfsdn_record_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cdfsdn_record,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cdfsdn_record,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cdfsdn_record_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CDFSDN_BLOCK_ptr,
        /* type_sizeof            */sizeof(CDFSDN_BLOCK) + sizeof(CDFSDN_CACHE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CDFSDN_BLOCK,
        /* init_type_func         */(UINT32)cdfsdn_block_init_0,
        /* clean_type_func        */(UINT32)cdfsdn_block_clean,
        /* free_type_func         */(UINT32)cdfsdn_block_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cdfsdn_block,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cdfsdn_block,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cdfsdn_block_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CBYTES_ptr,
        /* type_sizeof            */sizeof(CBYTES),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CBYTES,
        /* init_type_func         */(UINT32)cbytes_init,
        /* clean_type_func        */(UINT32)cbytes_clean,
        /* free_type_func         */(UINT32)cbytes_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cbytes,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cbytes,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cbytes_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_MOD_NODE_ptr,
        /* type_sizeof            */sizeof(MOD_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_MOD_NODE,
        /* init_type_func         */(UINT32)mod_node_init,
        /* clean_type_func        */(UINT32)mod_node_clean,
        /* free_type_func         */(UINT32)mod_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_mod_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_mod_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_mod_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CTIMET_ptr,
        /* type_sizeof            */sizeof(CTIMET),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CTIMET,
        /* init_type_func         */0,
        /* clean_type_func        */0,
        /* free_type_func         */0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_ctimet,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_ctimet,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_ctimet_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSESSION_NODE_ptr,
        /* type_sizeof            */sizeof(CSESSION_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSESSION_NODE,
        /* init_type_func         */(UINT32)csession_node_init,
        /* clean_type_func        */(UINT32)csession_node_clean,
        /* free_type_func         */(UINT32)csession_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csession_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csession_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csession_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSESSION_ITEM_ptr,
        /* type_sizeof            */sizeof(CSESSION_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSESSION_ITEM,
        /* init_type_func         */(UINT32)csession_item_init,
        /* clean_type_func        */(UINT32)csession_item_clean,
        /* free_type_func         */(UINT32)csession_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csession_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csession_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csession_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CLIST_ptr,
        /* type_sizeof            */sizeof(CLIST),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CLIST,
        /* init_type_func         */(UINT32)clist_init_0,
        /* clean_type_func        */(UINT32)clist_clean_0,
        /* free_type_func         */(UINT32)clist_free_0,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_clist,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_clist,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_clist_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSNP_KEY_ptr,
        /* type_sizeof            */sizeof(CRFSNP_KEY),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSNP_KEY,
        /* init_type_func         */(UINT32)crfsnp_key_init,
        /* clean_type_func        */(UINT32)crfsnp_key_clean,
        /* free_type_func         */(UINT32)crfsnp_key_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfsnp_key,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfsnp_key,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfsnp_key_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSNP_ITEM_ptr,
        /* type_sizeof            */sizeof(CRFSNP_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSNP_ITEM,
        /* init_type_func         */(UINT32)crfsnp_item_init,
        /* clean_type_func        */(UINT32)crfsnp_item_clean,
        /* free_type_func         */(UINT32)crfsnp_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfsnp_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfsnp_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfsnp_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSNP_FNODE_ptr,
        /* type_sizeof            */sizeof(CRFSNP_FNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSNP_FNODE,
        /* init_type_func         */(UINT32)crfsnp_fnode_init,
        /* clean_type_func        */(UINT32)crfsnp_fnode_clean,
        /* free_type_func         */(UINT32)crfsnp_fnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfsnp_fnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfsnp_fnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfsnp_fnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CXFSNP_KEY_ptr,
        /* type_sizeof            */sizeof(CXFSNP_KEY),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CXFSNP_KEY,
        /* init_type_func         */(UINT32)cxfsnp_key_init,
        /* clean_type_func        */(UINT32)cxfsnp_key_clean,
        /* free_type_func         */(UINT32)cxfsnp_key_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cxfsnp_key,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cxfsnp_key,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cxfsnp_key_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CXFSNP_ITEM_ptr,
        /* type_sizeof            */sizeof(CXFSNP_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CXFSNP_ITEM,
        /* init_type_func         */(UINT32)cxfsnp_item_init,
        /* clean_type_func        */(UINT32)cxfsnp_item_clean,
        /* free_type_func         */(UINT32)cxfsnp_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cxfsnp_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cxfsnp_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cxfsnp_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CXFSNP_FNODE_ptr,
        /* type_sizeof            */sizeof(CXFSNP_FNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CXFSNP_FNODE,
        /* init_type_func         */(UINT32)cxfsnp_fnode_init,
        /* clean_type_func        */(UINT32)cxfsnp_fnode_clean,
        /* free_type_func         */(UINT32)cxfsnp_fnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cxfsnp_fnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cxfsnp_fnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cxfsnp_fnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CXFS_NODE_ptr,
        /* type_sizeof            */sizeof(CXFS_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CXFS_NODE,
        /* init_type_func         */(UINT32)cxfs_node_init,
        /* clean_type_func        */(UINT32)cxfs_node_clean,
        /* free_type_func         */(UINT32)cxfs_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cxfs_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cxfs_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cxfs_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHFSNP_ITEM_ptr,
        /* type_sizeof            */sizeof(CHFSNP_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHFSNP_ITEM,
        /* init_type_func         */(UINT32)chfsnp_item_init,
        /* clean_type_func        */(UINT32)chfsnp_item_clean,
        /* free_type_func         */(UINT32)chfsnp_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chfsnp_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chfsnp_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chfsnp_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHFSNP_FNODE_ptr,
        /* type_sizeof            */sizeof(CHFSNP_FNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHFSNP_FNODE,
        /* init_type_func         */(UINT32)chfsnp_fnode_init,
        /* clean_type_func        */(UINT32)chfsnp_fnode_clean,
        /* free_type_func         */(UINT32)chfsnp_fnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chfsnp_fnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chfsnp_fnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chfsnp_fnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_uint64_t_ptr,
        /* type_sizeof            */sizeof(uint64_t),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_UINT64,
        /* init_type_func         */(UINT32)dbg_init_uint64_ptr,
        /* clean_type_func        */(UINT32)dbg_clean_uint64_ptr,
        /* free_type_func         */(UINT32)dbg_free_uint64_ptr,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_uint64_ptr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_uint64,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_uint64_ptr_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSDN_CACHE_NODE_ptr,
        /* type_sizeof            */sizeof(CRFSDN_CACHE_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSDN_CACHE_NODE,
        /* init_type_func         */(UINT32)crfsdn_cache_node_init,
        /* clean_type_func        */(UINT32)crfsdn_cache_node_clean,
        /* free_type_func         */(UINT32)crfsdn_cache_node_free,
        /* cmpi_encode_type_func  */(UINT32)/*cmpi_encode_crfsdn_cache_node*/NULL_PTR,
        /* cmpi_decode_type_func  */(UINT32)/*cmpi_decode_crfsdn_cache_node*/NULL_PTR,
        /* cmpi_encode_type_size  */(UINT32)/*cmpi_encode_crfsdn_cache_node_size*/NULL_PTR
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CMD5_DIGEST_ptr,
        /* type_sizeof            */sizeof(CMD5_DIGEST),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CMD5_DIGEST,
        /* init_type_func         */(UINT32)cmd5_digest_init,
        /* clean_type_func        */(UINT32)cmd5_digest_clean,
        /* free_type_func         */(UINT32)cmd5_digest_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cmd5_digest,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cmd5_digest,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cmd5_digest_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSOP_ptr,
        /* type_sizeof            */sizeof(CRFSOP),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSOP,
        /* init_type_func         */(UINT32)crfsop_init,
        /* clean_type_func        */(UINT32)crfsop_clean,
        /* free_type_func         */(UINT32)crfsop_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfsop,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfsop,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfsop_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFSDT_PNODE_ptr,
        /* type_sizeof            */sizeof(CRFSDT_PNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFSDT_PNODE,
        /* init_type_func         */(UINT32)crfsdt_pnode_init,
        /* clean_type_func        */(UINT32)crfsdt_pnode_clean,
        /* free_type_func         */(UINT32)crfsdt_pnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfsdt_pnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfsdt_pnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfsdt_pnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CBUFFER_ptr,
        /* type_sizeof            */sizeof(CBUFFER),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CBUFFER,
        /* init_type_func         */(UINT32)cbuffer_init,
        /* clean_type_func        */(UINT32)cbuffer_clean,
        /* free_type_func         */(UINT32)cbuffer_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cbuffer,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cbuffer,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cbuffer_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSTRKV_ptr,
        /* type_sizeof            */sizeof(CSTRKV),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSTRKV,
        /* init_type_func         */(UINT32)cstrkv_init_0,
        /* clean_type_func        */(UINT32)cstrkv_clean,
        /* free_type_func         */(UINT32)cstrkv_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cstrkv,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cstrkv,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cstrkv_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSTRKV_MGR_ptr,
        /* type_sizeof            */sizeof(CSTRKV_MGR),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSTRKV_MGR,
        /* init_type_func         */(UINT32)cstrkv_mgr_init,
        /* clean_type_func        */(UINT32)cstrkv_mgr_clean,
        /* free_type_func         */(UINT32)cstrkv_mgr_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cstrkv_mgr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cstrkv_mgr,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cstrkv_mgr_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHTTP_REQ_ptr,
        /* type_sizeof            */sizeof(CHTTP_REQ),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHTTP_REQ,
        /* init_type_func         */(UINT32)chttp_req_init,
        /* clean_type_func        */(UINT32)chttp_req_clean,
        /* free_type_func         */(UINT32)chttp_req_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chttp_req,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chttp_req,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chttp_req_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHTTP_RSP_ptr,
        /* type_sizeof            */sizeof(CHTTP_RSP),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHTTP_RSP,
        /* init_type_func         */(UINT32)chttp_rsp_init,
        /* clean_type_func        */(UINT32)chttp_rsp_clean,
        /* free_type_func         */(UINT32)chttp_rsp_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chttp_rsp,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chttp_rsp,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chttp_rsp_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHTTP_STAT_ptr,
        /* type_sizeof            */sizeof(CHTTP_STAT),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHTTP_STAT,
        /* init_type_func         */(UINT32)chttp_stat_init,
        /* clean_type_func        */(UINT32)chttp_stat_clean,
        /* free_type_func         */(UINT32)chttp_stat_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chttp_stat,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chttp_stat,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chttp_stat_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHTTP_STORE_ptr,
        /* type_sizeof            */sizeof(CHTTP_STORE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHTTP_STORE,
        /* init_type_func         */(UINT32)chttp_store_init,
        /* clean_type_func        */(UINT32)chttp_store_clean,
        /* free_type_func         */(UINT32)chttp_store_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chttp_store,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chttp_store,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chttp_store_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_TASKS_NODE_ptr,
        /* type_sizeof            */sizeof(TASKS_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_TASKS_NODE,
        /* init_type_func         */(UINT32)tasks_node_init_0,
        /* clean_type_func        */(UINT32)tasks_node_clean,
        /* free_type_func         */(UINT32)tasks_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_tasks_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_tasks_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_tasks_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CRFS_NODE_ptr,
        /* type_sizeof            */sizeof(CRFS_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CRFS_NODE,
        /* init_type_func         */(UINT32)crfs_node_init,
        /* clean_type_func        */(UINT32)crfs_node_clean,
        /* free_type_func         */(UINT32)crfs_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_crfs_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_crfs_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_crfs_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CHFS_NODE_ptr,
        /* type_sizeof            */sizeof(CHFS_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHFS_NODE,
        /* init_type_func         */(UINT32)chfs_node_init,
        /* clean_type_func        */(UINT32)chfs_node_clean,
        /* free_type_func         */(UINT32)chfs_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chfs_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chfs_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chfs_node_size
    );

    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSFSNP_ITEM_ptr,
        /* type_sizeof            */sizeof(CSFSNP_ITEM),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CHFSNP_ITEM,
        /* init_type_func         */(UINT32)chfsnp_item_init,
        /* clean_type_func        */(UINT32)chfsnp_item_clean,
        /* free_type_func         */(UINT32)chfsnp_item_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chfsnp_item,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chfsnp_item,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chfsnp_item_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSFSNP_FNODE_ptr,
        /* type_sizeof            */sizeof(CSFSNP_FNODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSFSNP_FNODE,
        /* init_type_func         */(UINT32)csfsnp_fnode_init,
        /* clean_type_func        */(UINT32)csfsnp_fnode_clean,
        /* free_type_func         */(UINT32)csfsnp_fnode_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_chfsnp_fnode,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_chfsnp_fnode,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_chfsnp_fnode_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CSFS_NODE_ptr,
        /* type_sizeof            */sizeof(CSFS_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CSFS_NODE,
        /* init_type_func         */(UINT32)csfs_node_init,
        /* clean_type_func        */(UINT32)csfs_node_clean,
        /* free_type_func         */(UINT32)csfs_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_csfs_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_csfs_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_csfs_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CTDNSSV_NODE_MGR_ptr,
        /* type_sizeof            */sizeof(CTDNSSV_NODE_MGR),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CTDNSSV_NODE_MGR,
        /* init_type_func         */(UINT32)ctdnssv_node_mgr_init,
        /* clean_type_func        */(UINT32)ctdnssv_node_mgr_clean,
        /* free_type_func         */(UINT32)ctdnssv_node_mgr_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_ctdnssv_node_mgr,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_ctdnssv_node_mgr,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_ctdnssv_node_mgr_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CTDNSSV_NODE_ptr,
        /* type_sizeof            */sizeof(CTDNSSV_NODE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CTDNSSV_NODE,
        /* init_type_func         */(UINT32)ctdnssv_node_init,
        /* clean_type_func        */(UINT32)ctdnssv_node_clean,
        /* free_type_func         */(UINT32)ctdnssv_node_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_ctdnssv_node,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_ctdnssv_node,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_ctdnssv_node_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CP2P_FILE_ptr,
        /* type_sizeof            */sizeof(CP2P_FILE),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CP2P_FILE,
        /* init_type_func         */(UINT32)cp2p_file_init,
        /* clean_type_func        */(UINT32)cp2p_file_clean,
        /* free_type_func         */(UINT32)cp2p_file_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cp2p_file,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cp2p_file,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cp2p_file_size
    );
    creg_type_conv_vec_add(type_conv_vec,
        /* type                   */e_dbg_CP2P_CMD_ptr,
        /* type_sizeof            */sizeof(CP2P_CMD),
        /* pointer_flag           */EC_TRUE,
        /* var_mm_type            */MM_CP2P_CMD,
        /* init_type_func         */(UINT32)cp2p_cmd_init,
        /* clean_type_func        */(UINT32)cp2p_cmd_clean,
        /* free_type_func         */(UINT32)cp2p_cmd_free,
        /* cmpi_encode_type_func  */(UINT32)cmpi_encode_cp2p_cmd,
        /* cmpi_decode_type_func  */(UINT32)cmpi_decode_cp2p_cmd,
        /* cmpi_encode_type_size  */(UINT32)cmpi_encode_cp2p_cmd_size
    );
    return (EC_TRUE);
}

FUNC_ADDR_MGR *creg_func_addr_mgr_new()
{
    FUNC_ADDR_MGR *func_addr_mgr;

    alloc_static_mem(MM_FUNC_ADDR_MGR, &func_addr_mgr, LOC_CREG_0005);
    if(NULL_PTR == func_addr_mgr)
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_func_addr_mgr_new: new func addr mgr failed\n");
        return (NULL_PTR);
    }
    creg_func_addr_mgr_init(func_addr_mgr);
    return (func_addr_mgr);
}

EC_BOOL creg_func_addr_mgr_init(FUNC_ADDR_MGR *func_addr_mgr)
{
    FUNC_ADDR_MGR_MD_TYPE(func_addr_mgr)            = MD_END;
    FUNC_ADDR_MGR_FUNC_NUM_PTR(func_addr_mgr)       = NULL_PTR;
    FUNC_ADDR_MGR_FUNC_ADDR_NODE(func_addr_mgr)     = NULL_PTR;
    FUNC_ADDR_MGR_MD_START_FUNC_ID(func_addr_mgr)   = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_END_FUNC_ID(func_addr_mgr)     = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_SET_MOD_FUNC_ID(func_addr_mgr) = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_GET_MOD_FUNC_ID(func_addr_mgr) = NULL_PTR;

    return (EC_TRUE);
}

EC_BOOL creg_func_addr_mgr_clean(FUNC_ADDR_MGR *func_addr_mgr)
{
    FUNC_ADDR_MGR_MD_TYPE(func_addr_mgr)            = MD_END;
    FUNC_ADDR_MGR_FUNC_NUM_PTR(func_addr_mgr)       = NULL_PTR;
    FUNC_ADDR_MGR_FUNC_ADDR_NODE(func_addr_mgr)     = NULL_PTR;
    FUNC_ADDR_MGR_MD_START_FUNC_ID(func_addr_mgr)   = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_END_FUNC_ID(func_addr_mgr)     = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_SET_MOD_FUNC_ID(func_addr_mgr) = ERR_FUNC_ID;
    FUNC_ADDR_MGR_MD_GET_MOD_FUNC_ID(func_addr_mgr) = NULL_PTR;
    return (EC_TRUE);
}

EC_BOOL creg_func_addr_mgr_free(FUNC_ADDR_MGR *func_addr_mgr)
{
    if(NULL_PTR != func_addr_mgr)
    {
        creg_func_addr_mgr_clean(func_addr_mgr);
        free_static_mem(MM_FUNC_ADDR_MGR, func_addr_mgr, LOC_CREG_0006);
    }
    return (EC_TRUE);
}

CVECTOR *creg_func_addr_vec_fetch()
{
    TASK_BRD *task_brd;
    task_brd = task_brd_default_get();
    return TASK_BRD_FUNC_ADDR_VEC(task_brd);
}

EC_BOOL creg_func_addr_vec_init(CVECTOR *func_addr_vec)
{
    cvector_init(func_addr_vec, CREG_FUNC_ADDR_MGR_DEFAULT_NUM, MM_FUNC_ADDR_MGR, CVECTOR_LOCK_ENABLE, LOC_CREG_0007);
    return (EC_TRUE);
}

EC_BOOL creg_func_addr_vec_clean(CVECTOR *func_addr_vec)
{
    cvector_clean(func_addr_vec, (CVECTOR_DATA_CLEANER)creg_func_addr_mgr_free, LOC_CREG_0008);
    return (EC_TRUE);
}

FUNC_ADDR_MGR *creg_func_addr_vec_get(const CVECTOR *func_addr_vec, const UINT32 md_type)
{
    return (FUNC_ADDR_MGR *)cvector_get(func_addr_vec, md_type);
}

EC_BOOL creg_func_addr_vec_add(CVECTOR *func_addr_vec,
                                        const UINT32 md_type, const UINT32 *func_num_ptr, const FUNC_ADDR_NODE *func_addr_node,
                                        const UINT32 md_start_func_id, const UINT32 md_end_func_id,
                                        const UINT32 md_set_mod_mgr_func_id, void * (*md_fget_mod_mgr)(const UINT32)
                                        )
{
    FUNC_ADDR_MGR *func_addr_mgr;
    UINT32 pos;

    if(NULL_PTR != cvector_get(func_addr_vec, md_type))
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_func_addr_vec_add: func addr mgr for md_type %ld was already defined\n", md_type);
        return (EC_FALSE);
    }

    for(pos = cvector_size(func_addr_vec); pos <= md_type; pos ++)
    {
        cvector_push(func_addr_vec, NULL_PTR);
    }

    func_addr_mgr = creg_func_addr_mgr_new();
    if(NULL_PTR == func_addr_mgr)
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_func_addr_vec_add: new type conv item failed\n");
        return (EC_FALSE);
    }

    FUNC_ADDR_MGR_MD_TYPE(func_addr_mgr)            = md_type;
    FUNC_ADDR_MGR_FUNC_NUM_PTR(func_addr_mgr)       = (UINT32 *)func_num_ptr;
    FUNC_ADDR_MGR_FUNC_ADDR_NODE(func_addr_mgr)     = (FUNC_ADDR_NODE *)func_addr_node;
    FUNC_ADDR_MGR_MD_START_FUNC_ID(func_addr_mgr)   = md_start_func_id;
    FUNC_ADDR_MGR_MD_END_FUNC_ID(func_addr_mgr)     = md_end_func_id;
    FUNC_ADDR_MGR_MD_SET_MOD_FUNC_ID(func_addr_mgr) = md_set_mod_mgr_func_id;
    FUNC_ADDR_MGR_MD_GET_MOD_FUNC_ID(func_addr_mgr) = md_fget_mod_mgr;

    cvector_set(func_addr_vec, md_type, (void *)func_addr_mgr);
    return (EC_TRUE);
}

EC_BOOL creg_func_addr_vec_add_default(CVECTOR *func_addr_vec)
{
    creg_func_addr_vec_add(func_addr_vec, MD_TASK   ,  NULL_PTR,                        NULL_PTR                                  , ERR_FUNC_ID       , ERR_FUNC_ID     , ERR_FUNC_ID             , NULL_PTR                               );
    creg_func_addr_vec_add(func_addr_vec, MD_TBD     ,  &g_tbd_func_addr_list_len    ,   (FUNC_ADDR_NODE *)g_tbd_func_addr_list    , FI_tbd_start     , FI_tbd_end      , FI_tbd_set_mod_mgr      , (dbg_md_fget_mod_mgr) tbd_get_mod_mgr      );
    creg_func_addr_vec_add(func_addr_vec, MD_CRUN    ,  &g_crun_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_crun_func_addr_list   , ERR_FUNC_ID      , ERR_FUNC_ID     , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_SUPER   ,  &g_super_func_addr_list_len  ,   (FUNC_ADDR_NODE *)g_super_func_addr_list  , FI_super_start   , FI_super_end    , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CTIMER  ,  NULL_PTR                     ,   NULL_PTR                                  , ERR_FUNC_ID      , ERR_FUNC_ID     , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CDFS    ,  &g_cdfs_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_cdfs_func_addr_list   , FI_cdfs_start    , FI_cdfs_end     , ERR_FUNC_ID             , (dbg_md_fget_mod_mgr) cdfs_get_dn_mod_mgr  );
    creg_func_addr_vec_add(func_addr_vec, MD_CBGT    ,  &g_cbgt_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_cbgt_func_addr_list   , FI_cbgt_start    , FI_cbgt_end     , FI_cbgt_set_mod_mgr     , (dbg_md_fget_mod_mgr) cbgt_get_mod_mgr     );
    creg_func_addr_vec_add(func_addr_vec, MD_CSESSION,  &g_csession_func_addr_list_len,  (FUNC_ADDR_NODE *)g_csession_func_addr_list, FI_csession_start , FI_csession_end, ERR_FUNC_ID            , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CRFS    ,  &g_crfs_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_crfs_func_addr_list   , FI_crfs_start    , FI_crfs_end     , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CHFS    ,  &g_chfs_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_chfs_func_addr_list   , FI_chfs_start    , FI_chfs_end     , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CSFS    ,  &g_csfs_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_csfs_func_addr_list   , FI_csfs_start    , FI_csfs_end     , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CRFSC   ,  &g_crfsc_func_addr_list_len  ,   (FUNC_ADDR_NODE *)g_crfsc_func_addr_list  , FI_crfsc_start   , FI_crfsc_end    , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CRFSMON ,  &g_crfsmon_func_addr_list_len,   (FUNC_ADDR_NODE *)g_crfsmon_func_addr_list, FI_crfsmon_start , FI_crfsmon_end  , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CHFSMON ,  &g_chfsmon_func_addr_list_len,   (FUNC_ADDR_NODE *)g_chfsmon_func_addr_list, FI_chfsmon_start , FI_chfsmon_end  , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CSFSMON ,  &g_csfsmon_func_addr_list_len,   (FUNC_ADDR_NODE *)g_csfsmon_func_addr_list, FI_csfsmon_start , FI_csfsmon_end  , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CTDNS   ,  &g_ctdns_func_addr_list_len  ,   (FUNC_ADDR_NODE *)g_ctdns_func_addr_list  , FI_ctdns_start   , FI_ctdns_end    , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CDETECT ,  &g_cdetect_func_addr_list_len,   (FUNC_ADDR_NODE *)g_cdetect_func_addr_list, FI_cdetect_start , FI_cdetect_end  , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CDETECTN,  &g_cdetectn_func_addr_list_len,   (FUNC_ADDR_NODE *)g_cdetectn_func_addr_list,FI_cdetectn_start, FI_cdetectn_end, ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CP2P    ,  &g_cp2p_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_cp2p_func_addr_list   , FI_cp2p_start    , FI_cp2p_end     , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CFILE   ,  &g_cfile_func_addr_list_len  ,   (FUNC_ADDR_NODE *)g_cfile_func_addr_list  , FI_cfile_start    , FI_cfile_end   , ERR_FUNC_ID             , NULL_PTR                                   );

    creg_func_addr_vec_add(func_addr_vec, MD_CXFS    ,  &g_cxfs_func_addr_list_len   ,   (FUNC_ADDR_NODE *)g_cxfs_func_addr_list   , FI_cxfs_start    , FI_cxfs_end     , ERR_FUNC_ID             , NULL_PTR                                   );
    creg_func_addr_vec_add(func_addr_vec, MD_CXFSMON ,  &g_cxfsmon_func_addr_list_len,   (FUNC_ADDR_NODE *)g_cxfsmon_func_addr_list, FI_cxfsmon_start , FI_cxfsmon_end  , ERR_FUNC_ID             , NULL_PTR                                   );

    return (EC_TRUE);
}


EC_BOOL creg_static_mem_tbl_add(const UINT32 mm_type, const char *mm_name, const UINT32 block_num, const UINT32 type_size, const UINT32 location)
{
    if(0 != reg_mm_man(mm_type, mm_name, block_num, type_size, location))
    {
        dbg_log(SEC_0123_CREG, 0)(LOGSTDOUT, "error:creg_static_mem_tbl_add: add static mem failed for type %ld, name %s, block num %ld, type size %ld at location %ld\n",
                            mm_type, mm_name, block_num, type_size, location);
        return (EC_FALSE);
    }
    return (EC_TRUE);
}


#ifdef __cplusplus
}
#endif/*__cplusplus*/

