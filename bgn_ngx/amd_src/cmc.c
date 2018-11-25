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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

#include <sys/stat.h>

#include "type.h"
#include "mm.h"
#include "log.h"

#include "cmisc.h"

#include "cbytes.h"
#include "cmc.h"

#if (SWITCH_ON == CMC_ASSERT_SWITCH)
#define CMC_ASSERT(condition)   ASSERT(condition)
#endif/*(SWITCH_ON == CMC_ASSERT_SWITCH)*/

#if (SWITCH_OFF == CMC_ASSERT_SWITCH)
#define CMC_ASSERT(condition)   do{}while(0)
#endif/*(SWITCH_OFF == CMC_ASSERT_SWITCH)*/

/**
*
* start CMC module
*
**/
CMC_MD *cmc_start(const UINT32 rdisk_size/*in GB*/, const UINT32 vdisk_size /*in MB*/)
{
    CMC_MD  *cmc_md;
    UINT32   page_max_num;

    UINT32   vdisk_num;
    uint8_t  np_model;

    init_static_mem();

    cmcpgd_model_search(vdisk_size, &vdisk_num);

    if(EC_FALSE == cmcnp_model_search(rdisk_size, &np_model))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_start: no np_model for physical disk %ld GB\n",
                                             rdisk_size);
        return (NULL_PTR);
    }

    /*1GB = 2^30 B*/
    page_max_num = rdisk_size * (UINT32_ONE << (30 - CMCPGB_PAGE_SIZE_NBITS));
    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_start: physical disk %ld GB, page size %u B => page num %ld\n",
                    rdisk_size, CMCPGB_PAGE_SIZE_NBYTES, page_max_num);

    /* create a new module node */
    cmc_md = safe_malloc(sizeof(CMC_MD), LOC_CMC_0001);
    if(NULL_PTR == cmc_md)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_start: start CMC failed\n");
        return (NULL_PTR);
    }

    /* initialize new one CMC module */
    CMC_MD_DN(cmc_md) = NULL_PTR;
    CMC_MD_NP(cmc_md) = NULL_PTR;

    if(EC_FALSE == cmc_create_np(cmc_md, (UINT32)np_model, page_max_num))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_start: CMC module %p create np failed\n", cmc_md);
        safe_free(cmc_md, LOC_CMC_0002);
        return (NULL_PTR);
    }

    if(EC_FALSE == cmc_create_dn(cmc_md, vdisk_num))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_start: CMC module %p create dn failed\n", cmc_md);

        cmc_close_np(cmc_md);

        safe_free(cmc_md, LOC_CMC_0003);
        return (NULL_PTR);
    }

    dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "[DEBUG] cmc_start: start CMC module %p\n", cmc_md);

    return (cmc_md);
}

/**
*
* end CMC module
*
**/
void cmc_end(CMC_MD *cmc_md)
{
    if(NULL_PTR == cmc_md)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_end: cmc %p not exist.\n", cmc_md);
        return;
    }

    cmc_close_np(cmc_md);
    cmc_close_dn(cmc_md);

    dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "cmc_end: stop CMC module %p\n", cmc_md);
    safe_free(cmc_md, LOC_CMC_0004);

    return ;
}

/**
*
* print CMC module
*
**/
void cmc_print(LOG *log, const CMC_MD *cmc_md)
{
    cmc_show_np(cmc_md, log);
    cmc_show_dn(cmc_md, log);

    return;
}

/**
*
* recycle deleted or retired space
*
**/
void cmc_process(CMC_MD *cmc_md)
{
    cmc_recycle(cmc_md, CMC_TRY_RECYCLE_MAX_NUM, NULL_PTR);
    return;
}

/**
*
*  create name node
*
**/
EC_BOOL cmc_create_np(CMC_MD *cmc_md, const UINT32 cmcnp_model, const UINT32 key_max_num)
{
    if(NULL_PTR != CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_np: np already exist\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == c_check_is_uint8_t(cmcnp_model))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_np: cmcnp_model %ld is invalid\n", cmcnp_model);
        return (EC_FALSE);
    }

    CMC_MD_NP(cmc_md) = cmcnp_create((uint32_t)0/*cmcnp_id*/, (uint8_t)cmcnp_model, key_max_num);
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_np: create np failed\n");
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  close name node
*
**/
EC_BOOL cmc_close_np(CMC_MD *cmc_md)
{
    if(NULL_PTR != CMC_MD_NP(cmc_md))
    {
        cmcnp_free(CMC_MD_NP(cmc_md));
        CMC_MD_NP(cmc_md) = NULL_PTR;
    }

    return (EC_TRUE);
}


/**
*
*  create data node
*
**/
EC_BOOL cmc_create_dn(CMC_MD *cmc_md, const UINT32 disk_num)
{
    if(NULL_PTR != CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_dn: dn already exist\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == c_check_is_uint16_t(disk_num))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_dn: disk_num %ld is invalid\n", disk_num);
        return (EC_FALSE);
    }

    CMC_MD_DN(cmc_md) = cmcdn_create((uint16_t)disk_num);
    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_create_dn: create dn failed\n");
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  close data node
*
**/
EC_BOOL cmc_close_dn(CMC_MD *cmc_md)
{
    if(NULL_PTR != CMC_MD_DN(cmc_md))
    {
        cmcdn_free(CMC_MD_DN(cmc_md));
        CMC_MD_DN(cmc_md) = NULL_PTR;
    }

    return (EC_TRUE);
}

STATIC_CAST static void __cmc_find_intersected_print(const CMCNP_KEY *cmcnp_key, const CMCNP_KEY *cmcnp_key_intersected, const CMCNP_KEY *cmcnp_key_next)
{
    sys_log(LOGSTDOUT, "[DEBUG] __cmc_find_intersected_print: key [%u, %u), intersected [%u, %u), next [%u, %u)\n",
                       CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key),
                       CMCNP_KEY_S_PAGE(cmcnp_key_intersected), CMCNP_KEY_E_PAGE(cmcnp_key_intersected),
                       CMCNP_KEY_S_PAGE(cmcnp_key_next), CMCNP_KEY_E_PAGE(cmcnp_key_next));
}
/**
*
*  find intersected range
*
**/
EC_BOOL cmc_find_intersected(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    CMCNP_ITEM       *cmcnp_item_intersected;
    CMCNP_KEY        *cmcnp_key_intersected;
    uint32_t          node_pos_intersected;

    if(EC_FALSE == cmcnp_key_is_valid(cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_find_intersected: invalid key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_find_intersected: np was not open\n");
        return (EC_FALSE);
    }

    node_pos_intersected = cmcnp_find_intersected(CMC_MD_NP(cmc_md), cmcnp_key, CMCNP_ITEM_FILE_IS_REG);
    if(CMCNPRB_ERR_POS == node_pos_intersected)
    {
        return (EC_FALSE);
    }

    cmcnp_item_intersected = cmcnp_fetch(CMC_MD_NP(cmc_md), node_pos_intersected);
    if(NULL_PTR == cmcnp_item_intersected)
    {
        return (EC_FALSE);
    }

    cmcnp_key_intersected = CMCNP_ITEM_KEY(cmcnp_item_intersected);

    if(CMCNP_KEY_S_PAGE(cmcnp_key) >= CMCNP_KEY_S_PAGE(cmcnp_key_intersected))
    {
        if(CMCNP_KEY_E_PAGE(cmcnp_key) >= CMCNP_KEY_E_PAGE(cmcnp_key_intersected))
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);

            if(CMCNP_KEY_S_PAGE(&cmcnp_key_next) < CMCNP_KEY_E_PAGE(&cmcnp_key_next))
            {
                __cmc_find_intersected_print(cmcnp_key, cmcnp_key_intersected, &cmcnp_key_next);

                cmc_find_intersected(cmc_md, &cmcnp_key_next);
            }

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key);

            if(CMCNP_KEY_S_PAGE(&cmcnp_key_next) < CMCNP_KEY_E_PAGE(&cmcnp_key_next))
            {
                __cmc_find_intersected_print(cmcnp_key, cmcnp_key_intersected, &cmcnp_key_next);

                cmc_find_intersected(cmc_md, &cmcnp_key_next);
            }
        }
        else
        {
            /*no next*/
        }
    }
    else
    {
        if(CMCNP_KEY_E_PAGE(cmcnp_key) >= CMCNP_KEY_E_PAGE(cmcnp_key_intersected))
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);

            if(CMCNP_KEY_S_PAGE(&cmcnp_key_next) < CMCNP_KEY_E_PAGE(&cmcnp_key_next))
            {
                __cmc_find_intersected_print(cmcnp_key, cmcnp_key_intersected, &cmcnp_key_next);

                cmc_find_intersected(cmc_md, &cmcnp_key_next);
            }

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key);

            if(CMCNP_KEY_S_PAGE(&cmcnp_key_next) < CMCNP_KEY_E_PAGE(&cmcnp_key_next))
            {
                __cmc_find_intersected_print(cmcnp_key, cmcnp_key_intersected, &cmcnp_key_next);

                cmc_find_intersected(cmc_md, &cmcnp_key_next);
            }
        }
        else
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);

            if(CMCNP_KEY_S_PAGE(&cmcnp_key_next) < CMCNP_KEY_E_PAGE(&cmcnp_key_next))
            {
                __cmc_find_intersected_print(cmcnp_key, cmcnp_key_intersected, &cmcnp_key_next);

                cmc_find_intersected(cmc_md, &cmcnp_key_next);
            }
        }
    }

    return (EC_TRUE);
}

/**
*
*  find closest range
*
**/
EC_BOOL cmc_find_closest(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, CMCNP_KEY *cmcnp_key_closest)
{
    uint32_t          node_pos_closest;

    if(EC_FALSE == cmcnp_key_is_valid(cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_find_closest: invalid key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_find_closest: np was not open\n");
        return (EC_FALSE);
    }

    node_pos_closest = cmcnp_find_closest(CMC_MD_NP(cmc_md), cmcnp_key, CMCNP_ITEM_FILE_IS_REG);
    if(CMCNPRB_ERR_POS == node_pos_closest)
    {
        return (EC_FALSE);
    }

    if(NULL_PTR != cmcnp_key_closest)
    {
        const CMCNP_ITEM *cmcnp_item_closest;

        cmcnp_item_closest = cmcnp_fetch(CMC_MD_NP(cmc_md), node_pos_closest);
        if(NULL_PTR == cmcnp_item_closest)
        {
            return (EC_FALSE);
        }
        cmcnp_key_clone(CMCNP_ITEM_KEY(cmcnp_item_closest), cmcnp_key_closest);
    }

    return (EC_TRUE);
}


/**
*
*  reserve space from dn
*
**/
STATIC_CAST static EC_BOOL __cmc_reserve_hash_dn(CMC_MD *cmc_md, const UINT32 data_len, const uint32_t path_hash, CMCNP_FNODE *cmcnp_fnode)
{
    CMCNP_INODE *cmcnp_inode;
    CMCPGV      *cmcpgv;

    uint32_t size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;
    uint16_t fail_tries;

    if(CMCPGB_SIZE_NBYTES <= data_len)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: data_len %ld overflow\n", data_len);
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: no dn was open\n");
        return (EC_FALSE);
    }

    if(NULL_PTR == CMCDN_CMCPGV(CMC_MD_DN(cmc_md)))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: no pgv exist\n");
        return (EC_FALSE);
    }

    cmcpgv = CMCDN_CMCPGV(CMC_MD_DN(cmc_md));
    if(NULL_PTR == CMCPGV_HEADER(cmcpgv))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: pgv header is null\n");
        return (EC_FALSE);
    }

    if(0 == CMCPGV_PAGE_DISK_NUM(cmcpgv))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: pgv has no disk yet\n");
        return (EC_FALSE);
    }

    fail_tries = 0;
    for(;;)
    {
        size    = (uint32_t)(data_len);
        disk_no = (uint16_t)(path_hash % CMCPGV_PAGE_DISK_NUM(cmcpgv));

        if(EC_TRUE == cmcpgv_new_space_from_disk(cmcpgv, size, disk_no, &block_no, &page_no))
        {
            break;/*fall through*/
        }

        /*try again*/
        if(EC_TRUE == cmcpgv_new_space(cmcpgv, size, &disk_no, &block_no, &page_no))
        {
            break;/*fall through*/
        }

        fail_tries ++;

        if(1 < fail_tries) /*try once only*/
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_hash_dn: "
                                                "new %ld bytes space from vol failed\n",
                                                data_len);
            return (EC_FALSE);
        }

        /*try to retire & recycle some files*/
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:__cmc_reserve_hash_dn: "
                                            "no %ld bytes space, try to retire & recycle\n",
                                            data_len);
        cmc_retire(cmc_md, (UINT32)CMC_TRY_RETIRE_MAX_NUM, NULL_PTR);
        cmc_recycle(cmc_md, (UINT32)CMC_TRY_RECYCLE_MAX_NUM, NULL_PTR);
    }

    cmcnp_fnode_init(cmcnp_fnode);
    CMCNP_FNODE_FILESZ(cmcnp_fnode) = size;
    CMCNP_FNODE_REPNUM(cmcnp_fnode) = 1;

    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    CMCNP_INODE_DISK_NO(cmcnp_inode)    = disk_no;
    CMCNP_INODE_BLOCK_NO(cmcnp_inode)   = block_no;
    CMCNP_INODE_PAGE_NO(cmcnp_inode)    = page_no;

    return (EC_TRUE);
}

/**
*
*  reserve space from dn
*
**/
EC_BOOL cmc_reserve_dn(CMC_MD *cmc_md, const UINT32 data_len, CMCNP_FNODE *cmcnp_fnode)
{
    CMCNP_INODE *cmcnp_inode;

    uint32_t size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

    if(CMCPGB_SIZE_NBYTES <= data_len)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_reserve_dn: data_len %ld overflow\n", data_len);
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_reserve_dn: no dn was open\n");
        return (EC_FALSE);
    }

    size = (uint32_t)(data_len);

    if(EC_FALSE == cmcpgv_new_space(CMCDN_CMCPGV(CMC_MD_DN(cmc_md)), size, &disk_no, &block_no, &page_no))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_reserve_dn: new %ld bytes space from vol failed\n", data_len);
        return (EC_FALSE);
    }

    cmcnp_fnode_init(cmcnp_fnode);
    CMCNP_FNODE_FILESZ(cmcnp_fnode) = size;
    CMCNP_FNODE_REPNUM(cmcnp_fnode) = 1;

    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    CMCNP_INODE_DISK_NO(cmcnp_inode)    = disk_no;
    CMCNP_INODE_BLOCK_NO(cmcnp_inode)   = block_no;
    CMCNP_INODE_PAGE_NO(cmcnp_inode)    = page_no;

    return (EC_TRUE);
}

/**
*
*  release space to dn
*
**/
EC_BOOL cmc_release_dn(CMC_MD *cmc_md, const CMCNP_FNODE *cmcnp_fnode)
{
    const CMCNP_INODE *cmcnp_inode;

    uint32_t file_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_release_dn: no dn was open\n");
        return (EC_FALSE);
    }

    file_size    = CMCNP_FNODE_FILESZ(cmcnp_fnode);
    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);

    if(CMCPGB_SIZE_NBYTES < file_size)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_release_dn: file_size %u overflow\n", file_size);
        return (EC_FALSE);
    }

    /*refer cmc_page_write: when file size is zero, only reserve np but no dn space*/
    if(0 == file_size)
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_release_dn: file_size is zero\n");
        return (EC_TRUE);/*Jan 4,2017 modify it from EC_FALSE to EC_TRUE*/
    }

    disk_no  = CMCNP_INODE_DISK_NO(cmcnp_inode) ;
    block_no = CMCNP_INODE_BLOCK_NO(cmcnp_inode);
    page_no  = CMCNP_INODE_PAGE_NO(cmcnp_inode) ;

    if(EC_FALSE == cmcpgv_free_space(CMCDN_CMCPGV(CMC_MD_DN(cmc_md)), disk_no, block_no, page_no, file_size))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_release_dn: free %u bytes to vol failed where disk %u, block %u, page %u\n",
                            file_size, disk_no, block_no, page_no);
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_release_dn: remove file fsize %u, disk %u, block %u, page %u done\n",
                       file_size, disk_no, block_no, page_no);

    return (EC_TRUE);
}

/**
*
*  reserve a fnode from name node
*
**/
STATIC_CAST static CMCNP_FNODE * __cmc_reserve_np(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    CMCNP_FNODE *cmcnp_fnode;

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_np: np was not open\n");
        return (NULL_PTR);
    }

    cmcnp_fnode = cmcnp_reserve(CMC_MD_NP(cmc_md), cmcnp_key);
    if(NULL_PTR == cmcnp_fnode)
    {
        /*try to retire & recycle some files*/
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "warn:__cmc_reserve_np: no name node accept key, try to retire & recycle\n");
        cmc_retire(cmc_md, (UINT32)CMC_TRY_RETIRE_MAX_NUM, NULL_PTR);
        cmc_recycle(cmc_md, (UINT32)CMC_TRY_RECYCLE_MAX_NUM, NULL_PTR);

        /*try again*/
        cmcnp_fnode = cmcnp_reserve(CMC_MD_NP(cmc_md), cmcnp_key);
        if(NULL_PTR == cmcnp_fnode)/*Oops!*/
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_reserve_np: no name node accept key\n");
            return (NULL_PTR);
        }
    }

    return (cmcnp_fnode);
}


/**
*
*  release a fnode from name node
*
**/
STATIC_CAST static EC_BOOL __cmc_release_np(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_release_np: np was not open\n");
        return (NULL_PTR);
    }

    if(EC_FALSE == cmcnp_release(CMC_MD_NP(cmc_md), cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:__cmc_release_np: release key from np failed\n");
        return (EC_FALSE);
    }
    return (EC_TRUE);
}

/**
*
*  locate a file and return base address of the first page
*
**/
UINT8 *cmc_file_locate(CMC_MD *cmc_md, UINT32 *offset, const UINT32 rsize)
{
    UINT32      s_offset;
    UINT32      e_offset;
    UINT32      s_page;
    UINT32      e_page;
    UINT8      *buff;

    s_offset = (*offset);
    e_offset = (*offset) + rsize;

    buff     = NULL_PTR; /*init*/

    s_page   = (s_offset >> CMCPGB_PAGE_SIZE_NBITS);
    e_page   = ((e_offset + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_locate: "
                                        "offset %ld, rsize %ld => offset [%ld, %ld) => page [%ld, %ld)\n",
                                        (*offset), rsize,
                                        s_offset, e_offset,
                                        s_page, e_page);

    for(; s_page < e_page; s_page ++)
    {
        CMCNP_KEY     cmcnp_key;
        UINT32        offset_t; /*offset in page*/
        UINT32        max_len;  /*max len in page*/
        UINT8        *m_buff;

        /*one page only*/
        CMCNP_KEY_S_PAGE(&cmcnp_key) = (uint32_t)(s_page + 0);
        CMCNP_KEY_E_PAGE(&cmcnp_key) = (uint32_t)(s_page + 1);

        if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), &cmcnp_key))
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_locate: miss page %ld\n",
                            s_page);
            break;
        }

        offset_t = (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK));
        max_len  = DMIN(e_offset - s_offset, CMCPGB_PAGE_SIZE_NBYTES - offset_t);

        m_buff = cmc_page_locate(cmc_md, &cmcnp_key);
        if(NULL_PTR == m_buff)
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_locate: locate page %ld failed\n",
                            s_page);
            break;
        }

        if(NULL_PTR == buff)
        {
            buff = m_buff;
        }

        s_offset += max_len;
    }

    (*offset) = s_offset;

    return (buff);
}

/**
*
*  read a file (POSIX style interface)
*
**/
EC_BOOL cmc_file_read(CMC_MD *cmc_md, UINT32 *offset, const UINT32 rsize, UINT8 *buff)
{
    UINT32      s_offset;
    UINT32      e_offset;
    UINT32      s_page;
    UINT32      e_page;
    UINT8      *m_buff;

    s_offset = (*offset);
    e_offset = (*offset) + rsize;
    m_buff   = buff;

    s_page   = (s_offset >> CMCPGB_PAGE_SIZE_NBITS);
    e_page   = ((e_offset + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_read: "
                                        "offset %ld, rsize %ld => offset [%ld, %ld) => page [%ld, %ld)\n",
                                        (*offset), rsize,
                                        s_offset, e_offset,
                                        s_page, e_page);

    for(; s_page < e_page; s_page ++)
    {
        CMCNP_KEY     cmcnp_key;
        UINT32        offset_t; /*offset in page*/
        UINT32        max_len;  /*max len in page*/
        CBYTES        cbytes;

        /*one page only*/
        CMCNP_KEY_S_PAGE(&cmcnp_key) = (uint32_t)(s_page + 0);
        CMCNP_KEY_E_PAGE(&cmcnp_key) = (uint32_t)(s_page + 1);

        if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), &cmcnp_key))
        {
            dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_file_read: mem miss page %ld\n",
                            s_page);
            break;
        }

        dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_read: mem hit page %ld\n",
                        s_page);

        offset_t = (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK));
        max_len  = DMIN(e_offset - s_offset, CMCPGB_PAGE_SIZE_NBYTES - offset_t);

        CBYTES_BUF(&cbytes) = m_buff;
        CBYTES_LEN(&cbytes) = e_offset - s_offset;

        if(EC_FALSE == cmc_page_read_e(cmc_md, &cmcnp_key, &offset_t, max_len, &cbytes))
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_read: "
                            "read page %ld, offset %ld, len %ld failed\n",
                            s_page, (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK)), max_len);
            return (EC_FALSE);
        }

        dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_read: "
                        "read page %ld => offset %ld, len %ld\n",
                        s_page, offset_t, CBYTES_LEN(&cbytes));

        CMC_ASSERT(CBYTES_BUF(&cbytes) == m_buff);

        s_offset += CBYTES_LEN(&cbytes);
        m_buff   += CBYTES_LEN(&cbytes);
    }

    (*offset) = s_offset;

    return (EC_TRUE);
}

/**
*
*  write a file (POSIX style interface)
*
**/
EC_BOOL cmc_file_write(CMC_MD *cmc_md, UINT32 *offset, const UINT32 wsize, UINT8 *buff)
{
    UINT32      s_offset;
    UINT32      e_offset;
    UINT32      s_page;
    UINT32      e_page;
    UINT8      *m_buff;

    s_offset = (*offset);
    e_offset = (*offset) + wsize;
    m_buff   = buff;

    s_page   = (s_offset >> CMCPGB_PAGE_SIZE_NBITS);
    e_page   = ((e_offset + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_write: "
                                        "offset %ld, wsize %ld => offset [%ld, %ld) => page [%ld, %ld)\n",
                                        (*offset), wsize,
                                        s_offset, e_offset,
                                        s_page, e_page);

    for(; s_page < e_page; s_page ++)
    {
        CMCNP_KEY     cmcnp_key;
        UINT32        offset_t; /*offset in page*/
        UINT32        max_len;  /*max len in page*/
        CBYTES        cbytes;

        /*one page only*/
        CMCNP_KEY_S_PAGE(&cmcnp_key) = (uint32_t)(s_page + 0);
        CMCNP_KEY_E_PAGE(&cmcnp_key) = (uint32_t)(s_page + 1);

        offset_t = (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK));
        max_len  = DMIN(e_offset - s_offset, CMCPGB_PAGE_SIZE_NBYTES - offset_t);

        CBYTES_BUF(&cbytes) = m_buff;
        CBYTES_LEN(&cbytes) = max_len;

        /*when partial override, need  the whole page exists*/
        if(0 < offset_t || CMCPGB_PAGE_SIZE_NBYTES != max_len)
        {
            /*check existing*/
            if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), &cmcnp_key))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_write: "
                                "page %ld absent, offset %ld (%ld in page), len %ld\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            if(EC_FALSE == cmc_page_write_e(cmc_md, &cmcnp_key, &offset_t, max_len, &cbytes))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_write: "
                                "override page %ld, offset %ld (%ld in page), len %ld failed\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_write: "
                            "override page %ld, offset %ld (%ld in page), len %ld done\n",
                            s_page, s_offset, offset_t, max_len);
        }
        else
        {
            if(EC_FALSE == cmc_page_write(cmc_md, &cmcnp_key, &cbytes))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_write: "
                                "write page %ld, offset %ld (%ld in page), len %ld failed\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_write: "
                            "write page %ld, offset %ld (%ld in page), len %ld done\n",
                            s_page, s_offset, offset_t, max_len);
        }

        CMC_ASSERT(CBYTES_BUF(&cbytes) == m_buff);

        s_offset += CBYTES_LEN(&cbytes);
        m_buff   += CBYTES_LEN(&cbytes);
    }

    (*offset) = s_offset;

    return (EC_TRUE);
}

/**
*
*  delete a file (POSIX style interface)
*
**/
EC_BOOL cmc_file_delete(CMC_MD *cmc_md, UINT32 *offset, const UINT32 dsize)
{
    UINT32      s_offset;
    UINT32      e_offset;
    UINT32      s_page;
    UINT32      e_page;

    s_offset = (*offset);
    e_offset = (*offset) + dsize;

    s_page   = (s_offset >> CMCPGB_PAGE_SIZE_NBITS);
    e_page   = ((e_offset + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                                        "offset %ld, dsize %ld => offset [%ld, %ld) => page [%ld, %ld)\n",
                                        (*offset), dsize,
                                        s_offset, e_offset,
                                        s_page, e_page);

    for(; s_page < e_page; s_page ++)
    {
        CMCNP_KEY     cmcnp_key;
        UINT32        offset_t; /*offset in page*/
        UINT32        max_len;  /*max len in page*/

        /*one page only*/
        CMCNP_KEY_S_PAGE(&cmcnp_key) = (uint32_t)(s_page + 0);
        CMCNP_KEY_E_PAGE(&cmcnp_key) = (uint32_t)(s_page + 1);

        offset_t = (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK));
        max_len  = DMIN(e_offset - s_offset, CMCPGB_PAGE_SIZE_NBYTES - offset_t);

        /*skip non-existence*/
        if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), &cmcnp_key))
        {
            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                            "page %ld absent, [%ld, %ld), offset %ld, len %ld in page\n",
                            s_page,
                            s_offset, e_offset,
                            offset_t, max_len);
            s_offset += max_len;
            continue;
        }

        dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                        "mem hit page %ld, [%ld, %ld), offset %ld, len %ld in page\n",
                        s_page,
                        s_offset, e_offset,
                        offset_t, max_len);

        /*when partial delete, need the whole page exists*/
        if(0 < offset_t || CMCPGB_PAGE_SIZE_NBYTES != max_len)
        {
            CMCNP_FNODE   cmcnp_fnode;
            UINT32        file_size;

            cmcnp_fnode_init(&cmcnp_fnode);

            /*found inconsistency*/
            if(EC_FALSE == cmcnp_read(CMC_MD_NP(cmc_md), &cmcnp_key, &cmcnp_fnode))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_delete: "
                                "read page %ld failed, [%ld, %ld), offset %ld, len %ld in page\n",
                                s_page,
                                s_offset, e_offset,
                                offset_t, max_len);
                return (EC_FALSE);
            }

            file_size = CMCNP_FNODE_FILESZ(&cmcnp_fnode);

            if(file_size > offset_t + max_len)
            {
                /*do nothing*/
                dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                                "ignore page %ld (file size %ld > %ld + %ld), [%ld, %ld), offset %ld, len %ld in page\n",
                                s_page,
                                file_size, offset_t, max_len,
                                s_offset, e_offset,
                                offset_t, max_len);
            }

            else if (file_size <= offset_t)
            {
                /*do nothing*/
                dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                                "ignore page %ld (file size %ld <= %ld), [%ld, %ld), offset %ld, len %ld in page\n",
                                s_page,
                                file_size, offset_t,
                                s_offset, e_offset,
                                offset_t, max_len);
            }

            /*now: offset_t < file_size <= offset_t + max_len*/

            else if(0 == offset_t)
            {
                if(EC_FALSE == cmc_page_delete(cmc_md, &cmcnp_key))
                {
                    dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_delete: "
                                    "delete page %ld failed, [%ld, %ld), offset %ld, len %ld in page\n",
                                    s_page,
                                    s_offset, e_offset,
                                    offset_t, max_len);
                    return (EC_FALSE);
                }

                dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                                "delete page %ld done, [%ld, %ld), offset %ld, len %ld in page\n",
                                s_page,
                                s_offset, e_offset,
                                offset_t, max_len);
            }
            else
            {
                CMCNP_FNODE_FILESZ(&cmcnp_fnode) = (uint32_t)offset_t;

                if(EC_FALSE == cmcnp_update(CMC_MD_NP(cmc_md), &cmcnp_key, &cmcnp_fnode))
                {
                    dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_delete: "
                                    "update page %ld failed (file size %ld => %ld), [%ld, %ld), offset %ld, len %ld in page\n",
                                    s_page,
                                    file_size, offset_t,
                                    s_offset, e_offset,
                                    offset_t, max_len);
                    return (EC_FALSE);
                }

                dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                                "update page %ld done (file size %ld => %ld), [%ld, %ld), offset %ld, len %ld in page\n",
                                s_page,
                                file_size, offset_t,
                                s_offset, e_offset,
                                offset_t, max_len);
            }
        }

        else
        {
            if(EC_FALSE == cmc_page_delete(cmc_md, &cmcnp_key))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_delete: "
                                "delete page %ld failed, [%ld, %ld), offset %ld, len %ld\n",
                                s_page,
                                s_offset, e_offset,
                                offset_t, max_len);
                return (EC_FALSE);
            }

            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_delete: "
                            "delete page %ld done, [%ld, %ld), offset %ld, len %ld\n",
                            s_page,
                            s_offset, e_offset,
                            offset_t, max_len);
        }

        s_offset += max_len;
    }

    (*offset) = s_offset;

    return (EC_TRUE);
}

/**
*
*  set file flush flag which means flush it to ssd when retire
*
**/
EC_BOOL cmc_file_set_flush(CMC_MD *cmc_md, UINT32 *offset, const UINT32 wsize)
{
    UINT32      s_offset;
    UINT32      e_offset;
    UINT32      s_page;
    UINT32      e_page;

    s_offset = (*offset);
    e_offset = (*offset) + wsize;

    s_page   = (s_offset >> CMCPGB_PAGE_SIZE_NBITS);
    e_page   = ((e_offset + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_set_flush: "
                                        "offset %ld, wsize %ld => offset [%ld, %ld) => page [%ld, %ld)\n",
                                        (*offset), wsize,
                                        s_offset, e_offset,
                                        s_page, e_page);

    for(; s_page < e_page; s_page ++)
    {
        CMCNP_KEY     cmcnp_key;
        UINT32        offset_t; /*offset in page*/
        UINT32        max_len;  /*max len in page*/

        /*one page only*/
        CMCNP_KEY_S_PAGE(&cmcnp_key) = (uint32_t)(s_page + 0);
        CMCNP_KEY_E_PAGE(&cmcnp_key) = (uint32_t)(s_page + 1);

        offset_t = (s_offset & ((UINT32)CMCPGB_PAGE_SIZE_MASK));
        max_len  = DMIN(e_offset - s_offset, CMCPGB_PAGE_SIZE_NBYTES - offset_t);

        /*when partial override, need  the whole page exists*/
        if(0 < offset_t || CMCPGB_PAGE_SIZE_NBYTES != max_len)
        {
            /*check existing*/
            if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), &cmcnp_key))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_set_flush: "
                                "page %ld absent, offset %ld (%ld in page), len %ld\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            if(EC_FALSE == cmcnp_set_flush(CMC_MD_NP(cmc_md), &cmcnp_key))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_set_flush: "
                                "set flush flag of page %ld, offset %ld (%ld in page), len %ld failed\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_set_flush: "
                            "set flush flag of page %ld, offset %ld (%ld in page), len %ld done\n",
                            s_page, s_offset, offset_t, max_len);
        }
        else
        {
            if(EC_FALSE == cmcnp_set_flush(CMC_MD_NP(cmc_md), &cmcnp_key))
            {
                dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_set_flush: "
                                "set flush flag of  page %ld, offset %ld (%ld in page), len %ld failed\n",
                                s_page, s_offset, offset_t, max_len);
                return (EC_FALSE);
            }

            dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_set_flush: "
                            "set flush flag of  page %ld, offset %ld (%ld in page), len %ld done\n",
                            s_page, s_offset, offset_t, max_len);
        }

        s_offset += max_len;
    }

    (*offset) = s_offset;

    return (EC_TRUE);
}

/**
*
*  locate a page
*
**/
UINT8 *cmc_page_locate(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    CMCNP_FNODE  *cmcnp_fnode;
    CMCNP_INODE  *cmcnp_inode;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_locate: np was not open\n");
        return (NULL_PTR);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_locate: dn was not open\n");
        return (NULL_PTR);
    }

    cmcnp_fnode = cmcnp_locate(CMC_MD_NP(cmc_md), cmcnp_key);
    if(NULL_PTR == cmcnp_fnode)
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_page_locate: locate np failed\n");
        return (NULL_PTR);
    }

    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);

    return cmcdn_node_locate(CMC_MD_DN(cmc_md),
                            CMCNP_INODE_DISK_NO(cmcnp_inode),
                            CMCNP_INODE_BLOCK_NO(cmcnp_inode),
                            CMCNP_INODE_PAGE_NO(cmcnp_inode));
}

/**
*
*  write a page
*
**/
EC_BOOL cmc_page_write(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, const CBYTES *cbytes)
{
    CMCNP_FNODE  *cmcnp_fnode;
    UINT32        page_num;
    UINT32        space_len;
    UINT32        data_len;
    uint32_t      path_hash;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    cmcnp_fnode = __cmc_reserve_np(cmc_md, cmcnp_key);
    if(NULL_PTR == cmcnp_fnode)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write: reserve np failed\n");

        return (EC_FALSE);
    }

    path_hash = cmcnp_key_hash(cmcnp_key);

    /*exception*/
    if(0 == CBYTES_LEN(cbytes))
    {
        cmcnp_fnode_init(cmcnp_fnode);
        CMCNP_FNODE_HASH(cmcnp_fnode) = path_hash;

        if(do_log(SEC_0118_CMC, 1))
        {
            sys_log(LOGSTDOUT, "warn:cmc_page_write: write with zero len to dn where fnode is \n");
            cmcnp_fnode_print(LOGSTDOUT, cmcnp_fnode);
        }

        return (EC_TRUE);
    }

    /*note: when reserve space from data node, the length depends on cmcnp_key but not cbytes*/
    page_num  = (CMCNP_KEY_E_PAGE(cmcnp_key) - CMCNP_KEY_S_PAGE(cmcnp_key));
    space_len = (page_num << CMCPGB_PAGE_SIZE_NBITS);
    data_len  = DMIN(space_len, CBYTES_LEN(cbytes));/*xxx*/

    /*when fnode is duplicate, do not reserve data node anymore*/
    if(0 == CMCNP_FNODE_REPNUM(cmcnp_fnode))
    {
        if(EC_FALSE == __cmc_reserve_hash_dn(cmc_md, data_len, path_hash, cmcnp_fnode))
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write: reserve dn %ld bytes failed\n",
                            data_len);

            __cmc_release_np(cmc_md, cmcnp_key);

            return (EC_FALSE);
        }
        CMCNP_FNODE_HASH(cmcnp_fnode)   = path_hash;
    }
    else
    {
        /*when fnode is duplicate, update file size*/
        CMCNP_FNODE_FILESZ(cmcnp_fnode) = data_len;
    }

    if(EC_FALSE == cmc_export_dn(cmc_md, cbytes, cmcnp_fnode))
    {
        cmc_release_dn(cmc_md, cmcnp_fnode);

        __cmc_release_np(cmc_md, cmcnp_key);

        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write: export content to dn failed\n");

        return (EC_FALSE);
    }

    if(do_log(SEC_0118_CMC, 9))
    {
        sys_log(LOGSTDOUT, "[DEBUG] cmc_page_write: write to dn where fnode is \n");
        cmcnp_fnode_print(LOGSTDOUT, cmcnp_fnode);
    }

    return (EC_TRUE);
}

/**
*
*  read a page
*
**/
EC_BOOL cmc_page_read(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, CBYTES *cbytes)
{
    CMCNP_FNODE   cmcnp_fnode;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    cmcnp_fnode_init(&cmcnp_fnode);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_read: read start\n");

    if(EC_FALSE == cmcnp_read(CMC_MD_NP(cmc_md), cmcnp_key, &cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_page_read: read from np failed\n");
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_read: read from np done\n");

    /*exception*/
    if(0 == CMCNP_FNODE_FILESZ(&cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_page_read: read with zero len from np and fnode %p is \n", &cmcnp_fnode);
        return (EC_TRUE);
    }

    if(EC_FALSE == cmc_read_dn(cmc_md, &cmcnp_fnode, cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_read: read from dn failed where fnode is \n");
        cmcnp_fnode_print(LOGSTDOUT, &cmcnp_fnode);
        return (EC_FALSE);
    }

    if(do_log(SEC_0118_CMC, 9))
    {
        sys_log(LOGSTDOUT, "[DEBUG] cmc_page_read: read with size %ld done\n",
                            cbytes_len(cbytes));
        cmcnp_fnode_print(LOGSTDOUT, &cmcnp_fnode);
    }
    return (EC_TRUE);
}

/*----------------------------------- POSIX interface -----------------------------------*/
/**
*
*  write a page at offset
*
**/
EC_BOOL cmc_page_write_e(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, UINT32 *offset, const UINT32 max_len, const CBYTES *cbytes)
{
    CMCNP_FNODE   cmcnp_fnode;
    uint32_t      file_old_size;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    cmcnp_fnode_init(&cmcnp_fnode);

    if(EC_FALSE == cmcnp_read(CMC_MD_NP(cmc_md), cmcnp_key, &cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write_e: read from np failed\n");
        return (EC_FALSE);
    }

    file_old_size = CMCNP_FNODE_FILESZ(&cmcnp_fnode);

    if(EC_FALSE == cmc_write_e_dn(cmc_md, &cmcnp_fnode, offset, max_len, cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write_e: offset write to dn failed\n");
        return (EC_FALSE);
    }

    if(file_old_size != CMCNP_FNODE_FILESZ(&cmcnp_fnode))
    {
        if(EC_FALSE == cmcnp_update(CMC_MD_NP(cmc_md), cmcnp_key, &cmcnp_fnode))
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_write_e: offset write to np failed\n");
            return (EC_FALSE);
        }
    }

    return (EC_TRUE);
}

/**
*
*  read a page from offset
*
*  when max_len = 0, return the partial content from offset to EOF (end of file)
*
**/
EC_BOOL cmc_page_read_e(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, UINT32 *offset, const UINT32 max_len, CBYTES *cbytes)
{
    CMCNP_FNODE   cmcnp_fnode;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    cmcnp_fnode_init(&cmcnp_fnode);

    if(EC_FALSE == cmcnp_read(CMC_MD_NP(cmc_md), cmcnp_key, &cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_read_e: read from np failed\n");
        return (EC_FALSE);
    }

    if(do_log(SEC_0118_CMC, 9))
    {
        sys_log(LOGSTDOUT, "[DEBUG] cmc_page_read_e: read from np and fnode %p is \n",
                           &cmcnp_fnode);
        cmcnp_fnode_print(LOGSTDOUT, &cmcnp_fnode);
    }

    /*exception*/
    if(0 == CMCNP_FNODE_FILESZ(&cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_page_read_e: read with zero len from np and fnode %p is \n", &cmcnp_fnode);
        cmcnp_fnode_print(LOGSTDOUT, &cmcnp_fnode);
        return (EC_TRUE);
    }

    if(EC_FALSE == cmc_read_e_dn(cmc_md, &cmcnp_fnode, offset, max_len, cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_read_e: offset read from dn failed where fnode is\n");
        cmcnp_fnode_print(LOGSTDOUT, &cmcnp_fnode);
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  export data into data node
*
**/
EC_BOOL cmc_export_dn(CMC_MD *cmc_md, const CBYTES *cbytes, const CMCNP_FNODE *cmcnp_fnode)
{
    const CMCNP_INODE *cmcnp_inode;

    UINT32   offset;
    UINT32   data_len;
    //uint32_t size;

    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

    data_len = DMIN(CBYTES_LEN(cbytes), CMCNP_FNODE_FILESZ(cmcnp_fnode));

    if(CMCPGB_SIZE_NBYTES <= data_len)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_export_dn: CBYTES_LEN %u or CMCNP_FNODE_FILESZ %u overflow\n",
                            (uint32_t)CBYTES_LEN(cbytes), CMCNP_FNODE_FILESZ(cmcnp_fnode));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_export_dn: no dn was open\n");
        return (EC_FALSE);
    }

    //size = (uint32_t)data_len;

    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    disk_no  = CMCNP_INODE_DISK_NO(cmcnp_inode) ;
    block_no = CMCNP_INODE_BLOCK_NO(cmcnp_inode);
    page_no  = CMCNP_INODE_PAGE_NO(cmcnp_inode) ;

    offset  = (((UINT32)(page_no)) << (CMCPGB_PAGE_SIZE_NBITS));
    if(EC_FALSE == cmcdn_write_o(CMC_MD_DN(cmc_md), data_len, CBYTES_BUF(cbytes), disk_no, block_no, &offset))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_export_dn: write %ld bytes to disk %u block %u page %u failed\n",
                            data_len, disk_no, block_no, page_no);
        return (EC_FALSE);
    }
    //dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_export_dn: write %ld bytes to disk %u block %u page %u done\n",
    //                    data_len, disk_no, block_no, page_no);

    return (EC_TRUE);
}

/**
*
*  write data node
*
**/
EC_BOOL cmc_write_dn(CMC_MD *cmc_md, const CBYTES *cbytes, CMCNP_FNODE *cmcnp_fnode)
{
    CMCNP_INODE *cmcnp_inode;

    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

    if(CMCPGB_SIZE_NBYTES <= CBYTES_LEN(cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_dn: buff len (or file size) %ld overflow\n", CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_dn: no dn was open\n");
        return (EC_FALSE);
    }

    cmcnp_fnode_init(cmcnp_fnode);
    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);

    if(EC_FALSE == cmcdn_write_p(CMC_MD_DN(cmc_md), cbytes_len(cbytes), cbytes_buf(cbytes), &disk_no, &block_no, &page_no))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_dn: write %ld bytes to dn failed\n", CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }

    CMCNP_INODE_DISK_NO(cmcnp_inode)    = disk_no;
    CMCNP_INODE_BLOCK_NO(cmcnp_inode)   = block_no;
    CMCNP_INODE_PAGE_NO(cmcnp_inode)    = page_no;

    CMCNP_FNODE_FILESZ(cmcnp_fnode) = CBYTES_LEN(cbytes);
    CMCNP_FNODE_REPNUM(cmcnp_fnode) = 1;

    return (EC_TRUE);
}

/**
*
*  read data node
*
**/
EC_BOOL cmc_read_dn(CMC_MD *cmc_md, const CMCNP_FNODE *cmcnp_fnode, CBYTES *cbytes)
{
    const CMCNP_INODE *cmcnp_inode;

    uint32_t file_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_dn: dn is null\n");
        return (EC_FALSE);
    }

    if(0 == CMCNP_FNODE_REPNUM(cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_dn: no replica\n");
        return (EC_FALSE);
    }

    file_size   = CMCNP_FNODE_FILESZ(cmcnp_fnode);
    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    disk_no  = CMCNP_INODE_DISK_NO(cmcnp_inode) ;
    block_no = CMCNP_INODE_BLOCK_NO(cmcnp_inode);
    page_no  = CMCNP_INODE_PAGE_NO(cmcnp_inode) ;

    //dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_read_dn: file size %u, disk %u, block %u, page %u\n", file_size, disk_no, block_no, page_no);

    if(0 == CBYTES_LEN(cbytes))/*scenario: cbytes is not initialized*/
    {
        if(NULL_PTR != CBYTES_BUF(cbytes))
        {
            SAFE_FREE(CBYTES_BUF(cbytes), LOC_CMC_0005);
        }
        CBYTES_BUF(cbytes) = (UINT8 *)SAFE_MALLOC(file_size, LOC_CMC_0006);
        CBYTES_LEN(cbytes) = 0;
    }

    else if(CBYTES_LEN(cbytes) < (UINT32)file_size)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_dn: cbytes len %ld < file size %ld\n",
                        CBYTES_LEN(cbytes), (UINT32)file_size);
        return (EC_FALSE);
    }

    if(EC_FALSE == cmcdn_read_p(CMC_MD_DN(cmc_md), disk_no, block_no, page_no, file_size, CBYTES_BUF(cbytes), &(CBYTES_LEN(cbytes))))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_dn: read %u bytes from disk %u, block %u, page %u failed\n",
                           file_size, disk_no, block_no, page_no);
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  write data node at offset in the specific file
*
**/
EC_BOOL cmc_write_e_dn(CMC_MD *cmc_md, CMCNP_FNODE *cmcnp_fnode, UINT32 *offset, const UINT32 max_len, const CBYTES *cbytes)
{
    CMCNP_INODE *cmcnp_inode;

    uint32_t file_size;
    uint32_t file_max_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;
    uint32_t offset_t;

    UINT32   max_len_t;

    if(CMCPGB_SIZE_NBYTES <= (*offset) + CBYTES_LEN(cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_e_dn: offset %ld + buff len (or file size) %ld = %ld overflow\n",
                            (*offset), CBYTES_LEN(cbytes), (*offset) + CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_e_dn: no dn was open\n");
        return (EC_FALSE);
    }

    file_size   = CMCNP_FNODE_FILESZ(cmcnp_fnode);
    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    disk_no  = CMCNP_INODE_DISK_NO(cmcnp_inode) ;
    block_no = CMCNP_INODE_BLOCK_NO(cmcnp_inode);
    page_no  = CMCNP_INODE_PAGE_NO(cmcnp_inode) ;

    /*file_max_size = file_size alignment to one page*/
    file_max_size = (((file_size + CMCPGB_PAGE_SIZE_NBYTES - 1) >> CMCPGB_PAGE_SIZE_NBITS) << CMCPGB_PAGE_SIZE_NBITS);

    if(((UINT32)file_max_size) <= (*offset))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_e_dn: offset %ld overflow due to file max size is %u\n", (*offset), file_max_size);
        return (EC_FALSE);
    }

    offset_t  = (uint32_t)(*offset);
    max_len_t = DMIN(DMIN(max_len, file_max_size - offset_t), cbytes_len(cbytes));

    if(EC_FALSE == cmcdn_write_e(CMC_MD_DN(cmc_md), max_len_t, cbytes_buf(cbytes), disk_no, block_no, page_no, offset_t))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_write_e_dn: write %ld bytes to dn failed\n", CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }

    (*offset) += max_len_t;
    if((*offset) > file_size)
    {
        /*update file size info*/
        CMCNP_FNODE_FILESZ(cmcnp_fnode) = (uint32_t)(*offset);
    }

    return (EC_TRUE);
}

/**
*
*  read data node from offset in the specific file
*
**/
EC_BOOL cmc_read_e_dn(CMC_MD *cmc_md, const CMCNP_FNODE *cmcnp_fnode, UINT32 *offset, const UINT32 max_len, CBYTES *cbytes)
{
    const CMCNP_INODE *cmcnp_inode;

    uint32_t file_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;
    uint32_t offset_t;

    UINT32   max_len_t;

    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_e_dn: dn is null\n");
        return (EC_FALSE);
    }

    if(0 == CMCNP_FNODE_REPNUM(cmcnp_fnode))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_e_dn: no replica\n");
        return (EC_FALSE);
    }

    file_size   = CMCNP_FNODE_FILESZ(cmcnp_fnode);
    cmcnp_inode = CMCNP_FNODE_INODE(cmcnp_fnode, 0);
    disk_no  = CMCNP_INODE_DISK_NO(cmcnp_inode) ;
    block_no = CMCNP_INODE_BLOCK_NO(cmcnp_inode);
    page_no  = CMCNP_INODE_PAGE_NO(cmcnp_inode) ;

    if((*offset) >= file_size)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_e_dn: due to offset %ld >= file size %u\n", (*offset), file_size);
        return (EC_FALSE);
    }

    offset_t = (uint32_t)(*offset);
    if(0 == max_len)
    {
        max_len_t = file_size - offset_t;
    }
    else
    {
        max_len_t = DMIN(max_len, file_size - offset_t);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_read_e_dn: file size %u, disk %u, block %u, page %u, offset %u, max len %ld\n",
                        file_size, disk_no, block_no, page_no, offset_t, max_len_t);

    if(0 == CBYTES_LEN(cbytes))/*scenario: cbytes is not initialized*/
    {
        if(NULL_PTR != CBYTES_BUF(cbytes))
        {
            SAFE_FREE(CBYTES_BUF(cbytes), LOC_CMC_0007);
        }
        CBYTES_BUF(cbytes) = (UINT8 *)SAFE_MALLOC(max_len_t, LOC_CMC_0008);
        CBYTES_LEN(cbytes) = 0;
    }

    else if(CBYTES_LEN(cbytes) < max_len_t)
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_e_dn: cbytes len %ld < max len %ld\n",
                        CBYTES_LEN(cbytes), max_len_t);
        return (EC_FALSE);
    }

    if(EC_FALSE == cmcdn_read_e(CMC_MD_DN(cmc_md), disk_no, block_no, page_no, offset_t, max_len_t, CBYTES_BUF(cbytes), &(CBYTES_LEN(cbytes))))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_read_e_dn: read %ld bytes from disk %u, block %u, offset %u failed\n",
                           max_len_t, disk_no, block_no, offset_t);
        return (EC_FALSE);
    }

    (*offset) += CBYTES_LEN(cbytes);
    return (EC_TRUE);
}


/**
*
*  delete all intersected file
*
**/
EC_BOOL cmc_delete_intersected(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    CMCNP_ITEM       *cmcnp_item_intersected;
    CMCNP_KEY        *cmcnp_key_intersected;
    uint32_t          node_pos_intersected;

    if(EC_FALSE == cmcnp_key_is_valid(cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_delete_intersected: invalid key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_delete_intersected: np was not open\n");
        return (EC_FALSE);
    }

    node_pos_intersected = cmcnp_find_intersected(CMC_MD_NP(cmc_md), cmcnp_key, CMCNP_ITEM_FILE_IS_REG);
    if(CMCNPRB_ERR_POS == node_pos_intersected)
    {
        /*not found*/
        return (EC_TRUE);
    }

    cmcnp_item_intersected = cmcnp_fetch(CMC_MD_NP(cmc_md), node_pos_intersected);
    if(NULL_PTR == cmcnp_item_intersected)
    {
        return (EC_FALSE);
    }

    cmcnp_key_intersected = CMCNP_ITEM_KEY(cmcnp_item_intersected);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_delete_intersected: key [%u, %u), intersected [%u, %u) => delete\n",
                       CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key),
                       CMCNP_KEY_S_PAGE(cmcnp_key_intersected), CMCNP_KEY_E_PAGE(cmcnp_key_intersected));

    if(EC_FALSE == cmcnp_umount_item(CMC_MD_NP(cmc_md), node_pos_intersected))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_delete_intersected: umount failed\n");
        return (EC_FALSE);
    }

    if(CMCNP_KEY_S_PAGE(cmcnp_key) >= CMCNP_KEY_S_PAGE(cmcnp_key_intersected))
    {
        if(CMCNP_KEY_E_PAGE(cmcnp_key) >= CMCNP_KEY_E_PAGE(cmcnp_key_intersected))
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);

            cmc_delete_intersected(cmc_md, &cmcnp_key_next);

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key);

            cmc_delete_intersected(cmc_md, &cmcnp_key_next);
        }
        else
        {
            /*no next*/
        }
    }
    else
    {
        if(CMCNP_KEY_E_PAGE(cmcnp_key) >= CMCNP_KEY_E_PAGE(cmcnp_key_intersected))
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);

            cmc_delete_intersected(cmc_md, &cmcnp_key_next);

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key_intersected);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_E_PAGE(cmcnp_key);

            cmc_delete_intersected(cmc_md, &cmcnp_key_next);
        }
        else
        {
            CMCNP_KEY  cmcnp_key_next;

            CMCNP_KEY_S_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key);
            CMCNP_KEY_E_PAGE(&cmcnp_key_next) = CMCNP_KEY_S_PAGE(cmcnp_key_intersected);

            cmc_delete_intersected(cmc_md, &cmcnp_key_next);
        }
    }

    return (EC_TRUE);
}

/**
*
*  delete a page
*
**/
EC_BOOL cmc_page_delete(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    uint32_t     node_pos;

    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_page_delete: np was not open\n");
        return (EC_FALSE);
    }

    node_pos = cmcnp_search(CMC_MD_NP(cmc_md), cmcnp_key, CMCNP_ITEM_FILE_IS_REG);
    if(CMCNPRB_ERR_POS == node_pos)
    {
        /*not found*/

        dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_delete: cmc %p, not found key [%u, %u)\n",
                            cmc_md, CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));

        return (EC_TRUE);
    }

    if(EC_FALSE == cmcnp_umount_item(CMC_MD_NP(cmc_md), node_pos))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_delete: umount failed\n");
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_delete: cmc %p, key [%u, %u) done\n",
                        cmc_md, CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));

    return (EC_TRUE);
}

/**
*
*  update a page
*
**/
EC_BOOL cmc_page_update(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, const CBYTES *cbytes)
{
    CMC_ASSERT(CMCNP_KEY_S_PAGE(cmcnp_key) + 1 == CMCNP_KEY_E_PAGE(cmcnp_key));

    if(EC_FALSE == cmcnp_read(CMC_MD_NP(cmc_md), cmcnp_key, NULL_PTR))
    {
        /*file not exist, write as new file*/
        if(EC_FALSE == cmc_page_write(cmc_md, cmcnp_key, cbytes))
        {
            dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_update: write failed\n");
            return (EC_FALSE);
        }
        dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_update: write done\n");
        return (EC_TRUE);
    }

    /*file exist, update it*/
    if(EC_FALSE == cmc_page_delete(cmc_md, cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_update: delete old failed\n");
        return (EC_FALSE);
    }
    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_update: delete old done\n");

    if(EC_FALSE == cmc_page_write(cmc_md, cmcnp_key, cbytes))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_page_update: write new failed\n");
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_page_update: write new done\n");

    return (EC_TRUE);
}

/**
*
*  count file num under specific path
*  if path is regular file, return file_num 1
*  if path is directory, return file num under it
*
**/
EC_BOOL cmc_file_num(CMC_MD *cmc_md, UINT32 *file_num)
{
    uint32_t     file_num_t;

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_file_num: np was not open\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == cmcnp_file_num(CMC_MD_NP(cmc_md), &file_num_t))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_num: get file num of key failed\n");
        return (EC_FALSE);
    }

    if(NULL_PTR != file_num)
    {
        (*file_num) = file_num_t;
    }
    return (EC_TRUE);
}

/**
*
*  get file size of specific file given full path name
*
**/
EC_BOOL cmc_file_size(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key, UINT32 *file_size)
{
    if(EC_FALSE == cmcnp_key_is_valid(cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_size: invalid key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_file_size: np was not open\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == cmcnp_file_size(CMC_MD_NP(cmc_md), cmcnp_key, file_size))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_file_size: cmcnp mgr get size of key [%ld, %ld) failed\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_file_size: key [%ld, %ld), size %ld\n",
                    CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key), (*file_size));
    return (EC_TRUE);
}

/**
*
*  search in current name node
*
**/
EC_BOOL cmc_search(CMC_MD *cmc_md, const CMCNP_KEY *cmcnp_key)
{
    if(EC_FALSE == cmcnp_key_is_valid(cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_search: invalid key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_search: np was not open\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == cmcnp_has_key(CMC_MD_NP(cmc_md), cmcnp_key))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_search: miss key [%ld, %ld)\n",
                        CMCNP_KEY_S_PAGE(cmcnp_key), CMCNP_KEY_E_PAGE(cmcnp_key));
        return (EC_FALSE);
    }

    if(CMCNPRB_ERR_POS == cmcnp_search(CMC_MD_NP(cmc_md), cmcnp_key, CMCNP_ITEM_FILE_IS_REG))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_search: search failed\n");
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  empty recycle
*
**/
EC_BOOL cmc_recycle(CMC_MD *cmc_md, const UINT32 max_num, UINT32 *complete_num)
{
    CMCNP_RECYCLE_DN cmcnp_recycle_dn;
    UINT32           complete_recycle_num;

    dbg_log(SEC_0118_CMC, 8)(LOGSTDOUT, "[DEBUG] cmc_recycle: recycle beg\n");

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_recycle: np was not open\n");
        return (EC_FALSE);
    }

    CMCNP_RECYCLE_DN_ARG1(&cmcnp_recycle_dn)   = (void *)cmc_md;
    CMCNP_RECYCLE_DN_FUNC(&cmcnp_recycle_dn)   = (CMCNP_RECYCLE_DN_FUNC)cmc_release_dn;

    complete_recycle_num = 0;/*initialization*/

    if(EC_FALSE == cmcnp_recycle(CMC_MD_NP(cmc_md),  max_num, NULL_PTR, &cmcnp_recycle_dn, &complete_recycle_num))
    {
        dbg_log(SEC_0118_CMC, 0)(LOGSTDOUT, "error:cmc_recycle: recycle np failed\n");
        return (EC_FALSE);
    }

    dbg_log(SEC_0118_CMC, 8)(LOGSTDOUT, "[DEBUG] cmc_recycle: recycle end where complete %ld\n", complete_recycle_num);

    if(NULL_PTR != complete_num)
    {
        (*complete_num) = complete_recycle_num;
    }
    return (EC_TRUE);
}

/**
*
*  retire files
*
**/
EC_BOOL cmc_retire(CMC_MD *cmc_md, const UINT32 max_num, UINT32 *complete_num)
{
    UINT32      complete_retire_num;

    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_retire: np was not open\n");
        return (EC_FALSE);
    }

    complete_retire_num = 0;/*initialization*/

    cmcnp_retire(CMC_MD_NP(cmc_md), max_num, &complete_retire_num);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_retire: retire done where complete %ld\n", complete_retire_num);

    if(NULL_PTR != complete_num)
    {
        (*complete_num) = complete_retire_num;
    }

    return (EC_TRUE);
}

EC_BOOL cmc_set_retire_callback(CMC_MD *cmc_md, CMCNP_RETIRE_CALLBACK func, void *arg)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_set_retire_callback: np was not open\n");
        return (EC_FALSE);
    }

    return cmcnp_set_retire_callback(CMC_MD_NP(cmc_md), func, arg);
}


/**
*
*  show name node
*
*
**/
EC_BOOL cmc_show_np(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    cmcnp_print(log, CMC_MD_NP(cmc_md));

    return (EC_TRUE);
}

/**
*
*  show name node LRU
*
*
**/
EC_BOOL cmc_show_np_lru_list(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    cmcnp_print_lru_list(log, CMC_MD_NP(cmc_md));

    return (EC_TRUE);
}

/**
*
*  show name node DEL
*
*
**/
EC_BOOL cmc_show_np_del_list(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    cmcnp_print_del_list(log, CMC_MD_NP(cmc_md));

    return (EC_TRUE);
}

/**
*
*  show name node BITMAP
*
*
**/
EC_BOOL cmc_show_np_bitmap(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    cmcnp_print_bitmap(log, CMC_MD_NP(cmc_md));

    return (EC_TRUE);
}

/**
*
*  show cmcdn info if it is dn
*
*
**/
EC_BOOL cmc_show_dn(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_DN(cmc_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    cmcdn_print(log, CMC_MD_DN(cmc_md));

    return (EC_TRUE);
}

/**
*
*  show all files
*
**/

EC_BOOL cmc_show_files(const CMC_MD *cmc_md, LOG *log)
{
    if(NULL_PTR == CMC_MD_NP(cmc_md))
    {
        dbg_log(SEC_0118_CMC, 1)(LOGSTDOUT, "warn:cmc_show_files: np was not open\n");
        return (EC_FALSE);
    }

    cmcnp_walk(CMC_MD_NP(cmc_md), (CMCNPRB_WALKER)cmcnp_file_print, (void *)log);

    dbg_log(SEC_0118_CMC, 9)(LOGSTDOUT, "[DEBUG] cmc_show_files: walk cmcnp done\n");
    return (EC_TRUE);
}




#ifdef __cplusplus
}
#endif/*__cplusplus*/
