
#define EFC_ACCESS_MODE_128 0
#define FLASH_ACCESS_MODE_128 EFC_ACCESS_MODE_128
#define EFC0 (0x400E0A00U)
#define EFC ((Efc *)EFC0)

uint32_t ul_rc;
uint32_t unique_id[4];

typedef enum flash_rc {
  FLASH_RC_OK = 0,                   //!< Operation OK
  FLASH_RC_YES = 1,                  //!< Yes
  FLASH_RC_NO = 0,                   //!< No
  FLASH_RC_ERROR = 0x10,             //!< General error
  FLASH_RC_INVALID,                  //!< Invalid argument input
  FLASH_RC_NOT_SUPPORT = 0xFFFFFFFF  //!< Operation is not supported
} flash_rc_t;
typedef unsigned long UL;



void readUID_ComposeMAC(){
   ul_rc = efc_init(EFC, FLASH_ACCESS_MODE_128, 4);
  if (ul_rc != 0) {
    Serial.println(" FOUT");
  }

  Serial.println("-I- Reading 128 bits Unique Identifier\r");
  ul_rc = flash_read_unique_id(unique_id, 4);

  Serial.print("ID = ");
  Serial.print((UL)unique_id[0]);
  Serial.print(",");
  Serial.print((UL)unique_id[1]);
  Serial.print(",");
  Serial.print((UL)unique_id[2]);
  Serial.print(",");
  Serial.println((UL)unique_id[3]);

  uint32_t hash32 = unique_id[0];
  hash32 ^= unique_id[1];
  hash32 ^= unique_id[2];
  hash32 ^= unique_id[3];
  Serial.print(F("MAC is FE:ED:"));
  if (((hash32 >> 24) & 0xFF) < 0x10) Serial.print(F("0"));
  Serial.print((hash32 >> 24) & 0xFF, HEX);
  Serial.print(F(":"));
  if (((hash32 >> 16) & 0xFF) < 0x10) Serial.print(F("0"));
  Serial.print((hash32 >> 16) & 0xFF, HEX);
  Serial.print(F(":"));
  if (((hash32 >> 8) & 0xFF) < 0x10) Serial.print(F("0"));
  Serial.print((hash32 >> 8) & 0xFF, HEX);
  Serial.print(F(":"));
  if (((hash32 >> 0) & 0xFF) < 0x10) Serial.print(F("0"));
  Serial.println((hash32 >> 0) & 0xFF, HEX);
}// end readUID_ComposeMAC(

uint32_t flash_read_unique_id(uint32_t *pul_data, uint32_t ul_size) {
  uint32_t uid_buf[4];
  uint32_t ul_idx;

  if (FLASH_RC_OK != efc_perform_read_sequence(EFC, EFC_FCMD_STUI, EFC_FCMD_SPUI, uid_buf, 4)) {
    return FLASH_RC_ERROR;
  }

  if (ul_size > 4) {
    /* Only 4 dword to store unique ID */
    ul_size = 4;
  }

  for (ul_idx = 0; ul_idx < ul_size; ul_idx++) {
    pul_data[ul_idx] = uid_buf[ul_idx];
  }

  return FLASH_RC_OK;
}