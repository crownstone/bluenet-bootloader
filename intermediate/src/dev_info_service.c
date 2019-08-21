/**
 * Author: Dominik Egger
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: Oct 24, 2016
 * License: LGPLv3+
 */

#include <string.h>
#include "ble_gap.h"
#include "ble_srv_common.h"
#include "ble_dis.h"
#include "app_error.h"

#include "cfg/cs_HardwareVersions.h"

/**@brief     Function for initializing services that will be used by the application.
 */
void dev_info_service_init(void)
{

    uint32_t       err_code;

    // add device information service

    ble_dis_init_t dis_init_obj;
    memset(&dis_init_obj, 0, sizeof(dis_init_obj));

    ble_srv_ascii_to_utf8(&dis_init_obj.hw_rev_str, (char*)get_hardware_version());
    ble_srv_ascii_to_utf8(&dis_init_obj.fw_rev_str, BOOTLOADER_VERSION);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init_obj.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init_obj.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init_obj);

    APP_ERROR_CHECK(err_code);

}
