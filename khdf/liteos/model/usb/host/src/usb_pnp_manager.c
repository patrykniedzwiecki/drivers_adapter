/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_pnp_manager.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "usb_ddk_pnp_loader.h"

#define HDF_LOG_TAG usb_pnp_manager

bool UsbPnpManagerWriteModuleName(struct HdfSBuf *sbuf, const char *moduleName)
{
    return HdfSbufWriteString(sbuf, moduleName);
}

int32_t UsbPnpManagerRegisterOrUnregisterDevice(struct UsbPnpManagerDeviceInfo managerInfo)
{
    if (managerInfo.isReg) {
        struct HdfDeviceObject *devObj = HdfRegisterDevice(managerInfo.moduleName, managerInfo.serviceName,
            (const void *)managerInfo.privateData);
        if (devObj == NULL) {
            HDF_LOGE("%{public}s:%{public}d devObj is NULL!", __func__, __LINE__);
            return HDF_FAILURE;
        }
    } else {
        HdfUnregisterDevice(managerInfo.moduleName, managerInfo.serviceName);
    }

    return HDF_SUCCESS;
}

bool UsbPnpManagerAddPrivateData(struct HdfDeviceInfo *deviceInfo, const void *privateData)
{
    if (privateData != NULL) {
        deviceInfo->private = (const void *)OsalMemCalloc(sizeof(struct UsbPnpNotifyServiceInfo));
        if (deviceInfo->private != NULL) {
            uint32_t length = ((struct UsbPnpNotifyServiceInfo *)(privateData))->length;

            memcpy_s((void *)(deviceInfo->private), sizeof(struct UsbPnpNotifyServiceInfo), privateData, length);
            if (deviceInfo->private == NULL) {
                HDF_LOGE("%{public}s: memcpy_s private error", __func__);
                return false;
            }
        } else {
            HDF_LOGE("%{public}s: OsalMemCalloc private error", __func__);
            return false;
        }
    }

    return true;
}

static int32_t UsbPnpManagerDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL) {
        HDF_LOGE("%s:%d client is NULL, cmd = %d", __func__, __LINE__, cmd);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s:%d received cmd = %d", __func__, __LINE__, cmd);

    return UsbDdkPnpLoaderEventReceived(NULL, cmd, data);
}

static int32_t UsbPnpManagerBind(struct HdfDeviceObject *device)
{
    static struct IDeviceIoService pnpLoaderService = {
        .Dispatch = UsbPnpManagerDispatch,
    };

    dprintf("%s:%d enter!, device=%p\n", __func__, __LINE__, device);

    if (device == NULL) {
        dprintf("%s: device is NULL!\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    device->service = &pnpLoaderService;
    dprintf("%s:%d bind success\n", __func__, __LINE__);

    return HDF_SUCCESS;
}

static int32_t UsbPnpManagerInit(struct HdfDeviceObject *device)
{
    int ret;

    dprintf("%s:%d enter!, device=%p\n", __func__, __LINE__, device);

    if (device == NULL) {
        dprintf("%s: device is NULL\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = UsbDdkPnpLoaderEventHandle();
    if (ret != HDF_SUCCESS) {
        dprintf("%s:%d UsbPnpLoaderEventHandle error\n", __func__, __LINE__);
        return ret;
    }

    dprintf("%s:%d Init success\n", __func__, __LINE__);

    return ret;
}

static void UsbPnpManagerRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        dprintf("%s: device is null\n", __func__);
        return;
    }

    dprintf("%s:%d release success\n", __func__, __LINE__);

    return;
}

struct HdfDriverEntry g_usbPnpManagerEntry = {
    .moduleVersion = 1,
    .Bind = UsbPnpManagerBind,
    .Init = UsbPnpManagerInit,
    .Release = UsbPnpManagerRelease,
    .moduleName = "HDF_USB_PNP_MANAGER",
};

HDF_INIT(g_usbPnpManagerEntry);

