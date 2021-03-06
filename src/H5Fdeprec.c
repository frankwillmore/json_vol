/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*-------------------------------------------------------------------------
 *
 * Created:	H5Fdeprec.c
 *		October 1 2009
 *		Quincey Koziol <koziol@hdfgroup.org>
 *
 * Purpose:	Deprecated functions from the H5F interface.  These
 *              functions are here for compatibility purposes and may be
 *              removed in the future.  Applications should switch to the
 *              newer APIs.
 *
 *-------------------------------------------------------------------------
 */

/****************/
/* Module Setup */
/****************/

#include "H5Fmodule.h"          /* This source code file is part of the H5F module */


/***********/
/* Headers */
/***********/
#include "H5private.h"          /* Generic Functions                        */
#include "H5Eprivate.h"         /* Error handling                           */
#include "H5Fpkg.h"             /* File access                              */
#include "H5Iprivate.h"         /* IDs                                      */
#include "H5SMprivate.h"        /* Shared object header messages            */


/****************/
/* Local Macros */
/****************/


/******************/
/* Local Typedefs */
/******************/


/********************/
/* Package Typedefs */
/********************/


/********************/
/* Local Prototypes */
/********************/


/*********************/
/* Package Variables */
/*********************/


/*****************************/
/* Library Private Variables */
/*****************************/


/*******************/
/* Local Variables */
/*******************/


#ifndef H5_NO_DEPRECATED_SYMBOLS

/*-------------------------------------------------------------------------
 * Function:    H5Fget_info1
 *
 * Purpose:     Gets general information about the file, including:
 *              1. Get storage size for superblock extension if there is one.
 *              2. Get the amount of btree and heap storage for entries
 *                 in the SOHM table if there is one.
 *              3. The amount of free space tracked in the file.
 *
 * Return:      SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5Fget_info1(hid_t obj_id, H5F_info1_t *finfo)
{
    H5F_t *f;                           /* Top file in mount hierarchy */
    herr_t ret_value = SUCCEED;         /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE2("e", "i*x", obj_id, finfo);

    /* Check args */
    if (!finfo)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "no info struct")

    /* For file IDs, get the file object directly
     *
     * (This prevents the H5G_loc() call from returning the file pointer for
     * the top file in a mount hierarchy)
     */
    if (H5I_get_type(obj_id) == H5I_FILE ) {
        if (NULL == (f = (H5F_t *)H5I_object(obj_id)))
            HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a file")
    }
    else {
        H5G_loc_t     loc;        /* Object location */

        /* Get symbol table entry */
        if (H5G_loc(obj_id, &loc) < 0)
             HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "not a valid object ID")
        f = loc.oloc->file;
    }
    HDassert(f->shared);

    /* Reset file info struct */
    HDmemset (finfo, 0, sizeof(*finfo));

    /* Get the size of the superblock extension */
    if (H5F__super_size(f, H5AC_ind_read_dxpl_id, NULL, &finfo->super_ext_size) < 0)
        HGOTO_ERROR(H5E_FILE, H5E_CANTGET, FAIL, "Unable to retrieve superblock extension size")

    /* Check for SOHM info */
    if (H5F_addr_defined(f->shared->sohm_addr))
        if (H5SM_ih_size(f, H5AC_ind_read_dxpl_id, &finfo->sohm.hdr_size, &finfo->sohm.msgs_info) < 0)
            HGOTO_ERROR(H5E_FILE, H5E_CANTGET, FAIL, "Unable to retrieve SOHM index & heap storage info")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Fget_info1() */


/*-------------------------------------------------------------------------
 * Function:    H5Fis_hdf5
 *
 * Purpose:     Check the file signature to detect an HDF5 file.
 *
 * Bugs:        This function is not robust: it only uses the default file
 *              driver when attempting to open the file when in fact it
 *              should use all known file drivers.
 *
 * Return:      Success:    1 (true) or 0 (false)
 *
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
htri_t
H5Fis_hdf5(const char *name)
{
    htri_t      ret_value;              /* Return value */

    FUNC_ENTER_API((-1))
    H5TRACE1("t", "*s", name);

    /* Check args and all the boring stuff. */
    if (!name || !*name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, (-1), "no file name specified")

    /* call the private is_HDF5 function */
    if ((ret_value = H5F_is_hdf5(name, H5P_FILE_ACCESS_DEFAULT, H5AC_ind_read_dxpl_id, H5AC_rawdata_dxpl_id)) < 0)
        HGOTO_ERROR(H5E_FILE, H5E_NOTHDF5, (-1), "unable open file")

done:

    FUNC_LEAVE_API(ret_value)
} /* end H5Fis_hdf5() */

#endif /* H5_NO_DEPRECATED_SYMBOLS */

