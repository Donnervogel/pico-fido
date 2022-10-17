/*
 * This file is part of the Pico FIDO distribution (https://github.com/polhenarejos/pico-fido).
 * Copyright (c) 2022 Pol Henarejos.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ctap2_cbor.h"
#include "fido.h"
#include "ctap.h"
#include "files.h"
#include "apdu.h"
#include "version.h"

int cbor_get_info() {
    CborEncoder encoder, mapEncoder, arrayEncoder;
    CborError error = CborNoError;
    cbor_encoder_init(&encoder, ctap_resp->init.data + 1, CTAP_MAX_PACKET_SIZE, 0);
    CBOR_CHECK(cbor_encoder_create_map(&encoder, &mapEncoder, 10));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x01));
    CBOR_CHECK(cbor_encoder_create_array(&mapEncoder, &arrayEncoder, 3));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "U2F_V2"));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "FIDO_2_0"));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "FIDO_2_1"));
    CBOR_CHECK(cbor_encoder_close_container(&mapEncoder, &arrayEncoder));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x02));
    CBOR_CHECK(cbor_encoder_create_array(&mapEncoder, &arrayEncoder, 2));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "credProtect"));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "hmac-secret"));
    CBOR_CHECK(cbor_encoder_close_container(&mapEncoder, &arrayEncoder));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x03));
    CBOR_CHECK(cbor_encode_byte_string(&mapEncoder, aaguid, sizeof(aaguid)));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x04));
    CBOR_CHECK(cbor_encoder_create_map(&mapEncoder, &arrayEncoder, 5));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "rk"));
    CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, true));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "credMgmt"));
    CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, true));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "authnrCfg"));
    CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, true));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "clientPin"));
    if (file_has_data(ef_pin))
        CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, true));
    else
        CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, false));
    CBOR_CHECK(cbor_encode_text_stringz(&arrayEncoder, "pinUvAuthToken"));
    CBOR_CHECK(cbor_encode_boolean(&arrayEncoder, true));
    CBOR_CHECK(cbor_encoder_close_container(&mapEncoder, &arrayEncoder));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x06));
    CBOR_CHECK(cbor_encoder_create_array(&mapEncoder, &arrayEncoder, 2));
    CBOR_CHECK(cbor_encode_uint(&arrayEncoder, 1)); // PIN protocols
    CBOR_CHECK(cbor_encode_uint(&arrayEncoder, 2)); // PIN protocols
    CBOR_CHECK(cbor_encoder_close_container(&mapEncoder, &arrayEncoder));

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x07));
    CBOR_CHECK(cbor_encode_uint(&mapEncoder, MAX_CREDENTIAL_COUNT_IN_LIST)); // MAX_CRED_COUNT_IN_LIST

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x08));
    CBOR_CHECK(cbor_encode_uint(&mapEncoder, MAX_CRED_ID_LENGTH)); // MAX_CRED_ID_MAX_LENGTH

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x0D));
    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 4)); // minPINLength

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x0E));
    CBOR_CHECK(cbor_encode_uint(&mapEncoder, PICO_FIDO_VERSION)); // firmwareVersion

    CBOR_CHECK(cbor_encode_uint(&mapEncoder, 0x15));
    CBOR_CHECK(cbor_encoder_create_array(&mapEncoder, &arrayEncoder, 2));
    CBOR_CHECK(cbor_encode_uint(&arrayEncoder, CTAP_CONFIG_AUT));
    CBOR_CHECK(cbor_encode_uint(&arrayEncoder, CTAP_CONFIG_KEY_AGREEMENT));
    CBOR_CHECK(cbor_encoder_close_container(&mapEncoder, &arrayEncoder));

    CBOR_CHECK(cbor_encoder_close_container(&encoder, &mapEncoder));
    err:
    if (error != CborNoError)
        return -CTAP2_ERR_INVALID_CBOR;
    res_APDU_size = cbor_encoder_get_buffer_size(&encoder, res_APDU + 1);
    return 0;
}
