/** \file
 * packet related headers.
 */

#ifndef OPS_PACKET_H
#define OPS_PACKET_H

#include "configure.h"

#include <time.h>
#include <openssl/bn.h>
#include "types.h"
#include "errors.h"

/** General-use structure for variable-length data 
 */

typedef struct
    {
    size_t len;
    unsigned char *contents;
    } ops_data_t;

/************************************/
/* Packet Tags - RFC2440bis-12, 4.2 */
/************************************/

/** Packet Tag - Bit 7 Mask (this bit is always set).
 * The first byte of a packet is the "Packet Tag".  It always
 * has bit 7 set.  This is the mask for it.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_ALWAYS_SET		0x80

/** Packet Tag - New Format Flag.
 * Bit 6 of the Packet Tag is the packet format indicator.
 * If it is set, the new format is used, if cleared the
 * old format is used.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_NEW_FORMAT		0x40


/** Old Packet Format: Mask for content tag.
 * In the old packet format bits 5 to 2 (including)
 * are the content tag.  This is the mask to apply
 * to the packet tag.  Note that you need to
 * shift by #OPS_PTAG_OF_CONTENT_TAG_SHIFT bits.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_OF_CONTENT_TAG_MASK	0x3c
/** Old Packet Format: Offset for the content tag.
 * As described at #OPS_PTAG_OF_CONTENT_TAG_MASK the
 * content tag needs to be shifted after being masked
 * out from the Packet Tag.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_OF_CONTENT_TAG_SHIFT	2
/** Old Packet Format: Mask for length type.
 * Bits 1 and 0 of the packet tag are the length type
 * in the old packet format.
 *
 * See #ops_ptag_of_lt_t for the meaning of the values.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_OF_LENGTH_TYPE_MASK	0x03


/** Old Packet Format Lengths.
 * Defines the meanings of the 2 bits for length type in the
 * old packet format.
 *
 * \see RFC2440bis-12 4.2.1
 */
typedef enum
    {
    OPS_PTAG_OF_LT_ONE_BYTE		=0x00, /*!< Packet has a 1 byte length - header is 2 bytes long. */ 
    OPS_PTAG_OF_LT_TWO_BYTE		=0x01, /*!< Packet has a 2 byte length - header is 3 bytes long. */ 
    OPS_PTAG_OF_LT_FOUR_BYTE		=0x02, /*!< Packet has a 4 byte length - header is 5 bytes long. */ 
    OPS_PTAG_OF_LT_INDETERMINATE	=0x03  /*!< Packet has a indeterminate length. */ 
    } ops_ptag_of_lt_t;


/** New Packet Format: Mask for content tag.
 * In the new packet format the 6 rightmost bits
 * are the content tag.  This is the mask to apply
 * to the packet tag.  Note that you need to
 * shift by #OPS_PTAG_NF_CONTENT_TAG_SHIFT bits.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_NF_CONTENT_TAG_MASK	0x3f
/** New Packet Format: Offset for the content tag.
 * As described at #OPS_PTAG_NF_CONTENT_TAG_MASK the
 * content tag needs to be shifted after being masked
 * out from the Packet Tag.
 *
 * \see RFC2440bis-12 4.2
 */
#define OPS_PTAG_NF_CONTENT_TAG_SHIFT	0



/* PTag Content Tags */
/***************************/

/** Package Tags (aka Content Tags) and signature subpacket types.
 * This enumerates all rfc-defined packet tag values and the
 * signature subpacket type values that we understand.
 *
 * \see RFC2440bis-12 4.3
 * \see RFC2440bis-12 5.2.3.1
 */
enum ops_content_tag_t
    {
    OPS_PTAG_CT_RESERVED		= 0,	/*!< Reserved - a packet tag must not have this value */
    OPS_PTAG_CT_PK_SESSION_KEY		= 1,	/*!< Public-Key Encrypted Session Key Packet */
    OPS_PTAG_CT_SIGNATURE		= 2,	/*!< Signature Packet */
    OPS_PTAG_CT_SK_SESSION_KEY		= 3,	/*!< Symmetric-Key Encrypted Session Key Packet */
    OPS_PTAG_CT_ONE_PASS_SIGNATURE	= 4,	/*!< One-Pass Signature Packet */
    OPS_PTAG_CT_SECRET_KEY		= 5,	/*!< Secret Key Packet */
    OPS_PTAG_CT_PUBLIC_KEY		= 6,	/*!< Public Key Packet */
    OPS_PTAG_CT_SECRET_SUBKEY		= 7,	/*!< Secret Subkey Packet */
    OPS_PTAG_CT_COMPRESSED		= 8,	/*!< Compressed Data Packet */
    OPS_PTAG_CT_SK_DATA			= 9,	/*!< Symmetrically Encrypted Data Packet */
    OPS_PTAG_CT_MARKER			=10,	/*!< Marker Packet */
    OPS_PTAG_CT_LITERAL_DATA		=11,	/*!< Literal Data Packet */
    OPS_PTAG_CT_TRUST			=12,	/*!< Trust Packet */
    OPS_PTAG_CT_USER_ID			=13,	/*!< User ID Packet */
    OPS_PTAG_CT_PUBLIC_SUBKEY		=14,	/*!< Public Subkey Packet */
    OPS_PTAG_CT_RESERVED2		=15,	/*!< reserved */
    OPS_PTAG_CT_RESERVED3		=16,	/*!< reserved */
    OPS_PTAG_CT_USER_ATTRIBUTE		=17,	/*!< User Attribute Packet */
    OPS_PTAG_CT_SK_IP_DATA		=18,	/*!< Sym. Encrypted and Integrity Protected Data Packet */
    OPS_PTAG_CT_MDC			=19,	/*!< Modification Detection Code Packet */

    OPS_PARSER_ERROR			=0x100,	/*!< Internal Use: Parser Error */
    OPS_PARSER_PTAG			=0x101,	/*!< Internal Use: The packet is the "Packet Tag" itself - used when
						     callback sends back the PTag. */
    OPS_PTAG_RAW_SS			=0x102,	/*!< Internal Use: content is raw sig subtag */
    OPS_PTAG_SS_ALL			=0x103,	/*!< Internal Use: select all subtags */
    OPS_PARSER_PACKET_END		=0x104,
    OPS_PARSER_ERRCODE			=0x105, /*! < Internal Use: Parser Error with errcode returned */

    /* signature subpackets (0x200-2ff) (type+0x200) */
    /* only those we can parse are listed here */
    OPS_PTAG_SIGNATURE_SUBPACKET_BASE	=0x200,		/*!< Base for signature subpacket types - All signature type
							     values are relative to this value. */
    OPS_PTAG_SS_CREATION_TIME		=0x200+2,	/*!< signature creation time */
    OPS_PTAG_SS_EXPIRATION_TIME		=0x200+3,	/*!< signature expiration time */

    OPS_PTAG_SS_TRUST			=0x200+5,	/*!< trust signature */
    OPS_PTAG_SS_REGEXP			=0x200+6,	/*!< regular expression */
    OPS_PTAG_SS_REVOCABLE		=0x200+7,	/*!< revocable */
    OPS_PTAG_SS_KEY_EXPIRATION_TIME	=0x200+9,	/*!< key expiration time */
    OPS_PTAG_SS_RESERVED		=0x200+10,	/*!< reserved */
    OPS_PTAG_SS_PREFERRED_SKA 		=0x200+11,	/*!< preferred symmetric algorithms */
    OPS_PTAG_SS_REVOCATION_KEY 		=0x200+12,	/*!< revocation key */
    OPS_PTAG_SS_ISSUER_KEY_ID		=0x200+16, /*!< issuer key ID */
    OPS_PTAG_SS_NOTATION_DATA		=0x200+20, /*!< notation data */
    OPS_PTAG_SS_PREFERRED_HASH          =0x200+21, /*!< preferred hash algorithms */
    OPS_PTAG_SS_PREFERRED_COMPRESSION	=0x200+22, /*!< preferred compression algorithms */
    OPS_PTAG_SS_KEY_SERVER_PREFS	=0x200+23, /*!< key server preferences */
    OPS_PTAG_SS_PREFERRED_KEY_SERVER	=0x200+24, /*!< Preferred Key Server */
    OPS_PTAG_SS_PRIMARY_USER_ID		=0x200+25, /*!< primary User ID */
    OPS_PTAG_SS_POLICY_URL		=0x200+26, /*!< Policy URL */
    OPS_PTAG_SS_KEY_FLAGS 		=0x200+27, /*!< key flags */
    OPS_PTAG_SS_SIGNERS_USER_ID		=0x200+28, /*!< Signer's User ID */
    OPS_PTAG_SS_REVOCATION_REASON	=0x200+29, /*!< reason for revocation */
    OPS_PTAG_SS_FEATURES		=0x200+30, /*!< features */

    OPS_PTAG_SS_USERDEFINED00	=0x200+100, /*!< internal or user-defined */
    OPS_PTAG_SS_USERDEFINED01	=0x200+101, 
    OPS_PTAG_SS_USERDEFINED02	=0x200+102,
    OPS_PTAG_SS_USERDEFINED03	=0x200+103,
    OPS_PTAG_SS_USERDEFINED04	=0x200+104,
    OPS_PTAG_SS_USERDEFINED05	=0x200+105,
    OPS_PTAG_SS_USERDEFINED06	=0x200+106,
    OPS_PTAG_SS_USERDEFINED07	=0x200+107,
    OPS_PTAG_SS_USERDEFINED08	=0x200+108,
    OPS_PTAG_SS_USERDEFINED09	=0x200+109,
    OPS_PTAG_SS_USERDEFINED10	=0x200+110,

	
    /* pseudo content types */
    OPS_PTAG_CT_LITERAL_DATA_HEADER	=0x300,
    OPS_PTAG_CT_LITERAL_DATA_BODY	=0x300+1,
    OPS_PTAG_CT_SIGNATURE_HEADER	=0x300+2,
    OPS_PTAG_CT_SIGNATURE_FOOTER	=0x300+3,
    OPS_PTAG_CT_ARMOUR_HEADER		=0x300+4,
    OPS_PTAG_CT_ARMOUR_TRAILER		=0x300+5,
    OPS_PTAG_CT_SIGNED_CLEARTEXT_HEADER	=0x300+6,
    OPS_PTAG_CT_SIGNED_CLEARTEXT_BODY	=0x300+7,
    OPS_PTAG_CT_SIGNED_CLEARTEXT_TRAILER=0x300+8,
    OPS_PTAG_CT_UNARMOURED_TEXT		=0x300+9,
    };

/** Structure to hold one parse error string. */
typedef struct
    {
    const char *error; /*!< error message. */
    } ops_parser_error_t;

/** Structure to hold one error code */
typedef struct
    {
    ops_errcode_t errcode;
    } ops_parser_errcode_t;

/** Structure to hold one packet tag.
 * \see RFC2440bis-12 4.2
 */
typedef struct
    {
    unsigned		new_format;	/*!< Whether this packet tag is new (true) or old format (false) */
    unsigned		content_tag;	/*!< content_tag value - See #ops_content_tag_t for meanings */
    ops_ptag_of_lt_t	length_type;	/*!< Length type (#ops_ptag_of_lt_t) - only if this packet tag is old format.  Set to 0 if new format. */
    unsigned		length;		/*!< The length of the packet.  This value is set when we read and compute the
					  length information, not at the same moment we create the packet tag structure.
					  Only defined if #length_read is set. */  /* XXX: Ben, is this correct? */
    //    unsigned		length_read;	/*!< How much bytes of this packet we have read so far - for internal use
    //					  only. */
    unsigned		position;	/*!< The position (within the current reader) of the packet */
    } ops_ptag_t;

/** Public Key Algorithm Numbers.
 * OpenPGP assigns a unique Algorithm Number to each algorithm that is part of OpenPGP.
 *
 * This lists algorithm numbers for public key algorithms.
 * 
 * \see RFC2440bis-12 9.1
 */
typedef enum
    {
    OPS_PKA_RSA			=1,	/*!< RSA (Encrypt or Sign) */
    OPS_PKA_RSA_ENCRYPT_ONLY	=2,	/*!< RSA Encrypt-Only (deprecated - \see RFC2440bis-12 12.4) */
    OPS_PKA_RSA_SIGN_ONLY	=3,	/*!< RSA Sign-Only (deprecated - \see RFC2440bis-12 12.4) */
    OPS_PKA_ELGAMAL		=16,	/*!< Elgamal (Encrypt-Only) */
    OPS_PKA_DSA			=17,	/*!< DSA (Digital Signature Algorithm) */
    OPS_PKA_RESERVED_ELLIPTIC_CURVE	=18,	/*!< Reserved for Elliptic Curve */
    OPS_PKA_RESERVED_ECDSA		=19,	/*!< Reserved for ECDSA */
    OPS_PKA_ELGAMAL_ENCRYPT_OR_SIGN	=20, 	/*!< Deprecated. */
    OPS_PKA_RESERVED_DH			=21,	/*!< Reserved for Diffie-Hellman (X9.42, as defined for IETF-S/MIME) */
    OPS_PKA_PRIVATE00		=100,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE01		=101,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE02		=102,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE03		=103,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE04		=104,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE05		=105,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE06		=106,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE07		=107,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE08		=108,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE09		=109,	/*!< Private/Experimental Algorithm */
    OPS_PKA_PRIVATE10		=110,	/*!< Private/Experimental Algorithm */
    } ops_public_key_algorithm_t;

/** Structure to hold one DSA public key parameters.
 *
 * \see RFC2440bis-12 5.5.2
 */
typedef struct
    {
    BIGNUM *p;	/*!< DSA prime p */
    BIGNUM *q;	/*!< DSA group order q */
    BIGNUM *g;	/*!< DSA group generator g */
    BIGNUM *y;	/*!< DSA public key value y (= g^x mod p with x being the secret) */
    } ops_dsa_public_key_t;

/** Structure to hold on RSA public key.
 *
 * \see RFC2440bis-12 5.5.2
 */
typedef struct
    {
    BIGNUM *n;	/*!< RSA public modulus n */
    BIGNUM *e;	/*!< RSA public encryptiong exponent e */
    } ops_rsa_public_key_t;

/** Structure to hold on ElGamal public key parameters.
 *
 * \see RFC2440bis-12 5.5.2
 */
typedef struct
    {
    BIGNUM *p;	/*!< ElGamal prime p */
    BIGNUM *g;	/*!< ElGamal group generator g */
    BIGNUM *y;	/*!< ElGamal public key value y (= g^x mod p with x being the secret) */
    } ops_elgamal_public_key_t;

/** Union to hold public key parameters of any algorithm */
typedef union
    {
    ops_dsa_public_key_t	dsa;		/*!< A DSA public key */
    ops_rsa_public_key_t	rsa;		/*!< An RSA public key */
    ops_elgamal_public_key_t	elgamal;	/*!< An ElGamal public key */
    } ops_public_key_union_t;

/** Structure to hold one pgp public key */
typedef struct
    {
    unsigned 			version;	/*!< version of the key (v3, v4...) */
    time_t			creation_time;  /*!< when the key was created.  Note that interpretation varies with key
						  version. */
    unsigned			days_valid;	/*!< validity period of the key in days since creation.  A value of 0
						  has a special meaning indicating this key does not expire.  Only
						  used with v3 keys. */
    ops_public_key_algorithm_t	algorithm;	/*!< Public Key Algorithm type */
    ops_public_key_union_t	key;		/*!< Public Key Parameters */
    } ops_public_key_t;

/** Structure to hold data for one RSA secret key
 */
typedef struct
    {
    BIGNUM *d;
    BIGNUM *p;
    BIGNUM *q;
    BIGNUM *u;
    } ops_rsa_secret_key_t;

typedef struct
    {
    ops_rsa_secret_key_t rsa;
    } ops_secret_key_union_t;

typedef enum
    {
    OPS_S2K_NONE=0,
    } s2k_usage_t;

typedef struct
    {
    ops_public_key_t		public_key;
    s2k_usage_t			s2k_usage;
    unsigned			checksum;
    ops_secret_key_union_t	key;
    } ops_secret_key_t;

/** Symmetric Key Algorithm Numbers.
 * OpenPGP assigns a unique Algorithm Number to each algorithm that is part of OpenPGP.
 *
 * This lists algorithm numbers for symmetric key algorithms.
 * 
 * \see RFC2440bis-12 9.2
 */
typedef enum
    {
    OPS_SKA_PLAINTEXT	=0, /*!< Plaintext or unencrypted data */
    OPS_SKA_IDEA	=1, /*!< IDEA */
    OPS_SKA_TRIPLEDES	=2, /*!< TripleDES */
    OPS_SKA_CAST5	=3, /*!< CAST5 */
    OPS_SKA_BLOWFISH	=4, /*!< Blowfish */
    OPS_SKA_AES_128	=7, /*!< AES with 128-bit key (AES) */
    OPS_SKA_AES_192	=8, /*!< AES with 192-bit key */
    OPS_SKA_AES_256	=9, /*!< AES with 256-bit key */
    OPS_SKA_TWOFISH	=10, /*!< Twofish with 256-bit key (TWOFISH) */

    } ops_symmetric_key_algorithm_t;

/** Structure to hold one trust packet's data */

typedef struct
    {
    ops_data_t data; /*<! Trust Packet */
    } ops_trust_t;
	
/** Structure to hold one user id */
typedef struct
    {
    char *user_id;	/*!< User ID string */
    } ops_user_id_t;

/** Structure to hold one user attribute */
typedef struct
    {
    ops_data_t data; /*!< User Attribute */
    } ops_user_attribute_t;

/** Signature Version.
 * OpenPGP has two different signature versions: version 3 and version 4.
 *
 * \see RFC2440bis-12 5.2
 */
typedef enum
    {
    OPS_SIG_V3=3,	/*<! Version 3 Signature */
    OPS_SIG_V4=4,	/*<! Version 4 Signature */
    } ops_sig_version_t;

/** Signature Type.
 * OpenPGP defines different signature types that allow giving different meanings to signatures.  Signature types
 * include 0x10 for generitc User ID certifications (used when Ben signs Weasel's key), Subkey binding signatures,
 * document signatures, key revocations, etc.
 *
 * Different types are used in different places, and most make only sense in their intended location (for instance a
 * subkey binding has no place on a UserID).
 *
 * \see RFC2440bis-12 5.2.1
 */
typedef enum
    {
    OPS_SIG_BINARY	=0x00,	/*<! Signature of a binary document */
    OPS_SIG_TEXT	=0x01,	/*<! Signature of a canonical text document */
    OPS_SIG_STANDALONE	=0x02,	/*<! Standalone signature */

    OPS_CERT_GENERIC	=0x10,	/*<! Generic certification of a User ID and Public Key packet */
    OPS_CERT_PERSONA	=0x11,	/*<! Persona certification of a User ID and Public Key packet */
    OPS_CERT_CASUAL	=0x12,	/*<! Casual certification of a User ID and Public Key packet */
    OPS_CERT_POSITIVE	=0x13,	/*<! Positive certification of a User ID and Public Key packet */

    OPS_SIG_SUBKEY	=0x18,	/*<! Subkey Binding Signature */
    OPS_SIG_PRIMARY	=0x19,	/*<! Primary Key Binding Signature */
    OPS_SIG_DIRECT	=0x1f,	/*<! Signature directly on a key */

    OPS_SIG_REV_KEY	=0x20,	/*<! Key revocation signature */
    OPS_SIG_REV_SUBKEY	=0x28,	/*<! Subkey revocation signature */
    OPS_SIG_REV_CERT	=0x30,	/*<! Certification revocation signature */

    OPS_SIG_TIMESTAMP	=0x40,	/*<! Timestamp signature */

    OPS_SIG_3RD_PARTY	=0x50,	/*<! Third-Party Confirmation signature */
    } ops_sig_type_t;

/** Hashing Algorithm Numbers.
 * OpenPGP assigns a unique Algorithm Number to each algorithm that is part of OpenPGP.
 * 
 * This lists algorithm numbers for hash algorithms.
 *
 * \see RFC2440bis-12 9.4
 */
typedef enum
    {
    OPS_HASH_UNKNOWN	=-1,	/*!< used to indicate errors */
    OPS_HASH_MD5	= 1,	/*!< MD5 */
    OPS_HASH_SHA1	= 2,	/*!< SHA-1 */
    OPS_HASH_RIPEMD	= 3,	/*!< RIPEMD160 */

    OPS_HASH_SHA256	= 8,	/*!< SHA256 */
    OPS_HASH_SHA384	= 9,	/*!< SHA384 */
    OPS_HASH_SHA512	=10,	/*!< SHA512 */
    } ops_hash_algorithm_t;

/** Struct to hold parameters of an RSA signature */
typedef struct
    {
    BIGNUM			*sig;	/*!< the signature value (m^d % n) */
    } ops_rsa_signature_t;

/** Struct to hold parameters of a DSA signature */
typedef struct
    {
    BIGNUM			*r;	/*!< DSA value r */
    BIGNUM			*s;	/*!< DSA value s */
    } ops_dsa_signature_t;

typedef struct
    {
    BIGNUM			*r;
    BIGNUM			*s;
    } ops_elgamal_signature_t;

/** Struct to hold data for a private/experimental signature */
typedef struct
    {
    ops_data_t	data;
    } ops_unknown_signature_t;

/** Union to hold signature parameters of any algorithm */
typedef union
    {
    ops_rsa_signature_t		rsa;	/*!< An RSA Signature */
    ops_dsa_signature_t		dsa;	/*!< A DSA Signature */
    ops_elgamal_signature_t	elgamal; // deprecated
    ops_unknown_signature_t 	unknown; /* private or experimental */
    } ops_signature_union_t;

/** Struct to hold a signature packet.
 *
 * \see RFC2440bis-12 5.2.2
 * \see RFC2440bis-12 5.2.3
 */
#define OPS_KEY_ID_SIZE		8
/** signature */
typedef struct
    {
    ops_sig_version_t		version;	/*!< signature version number */
    ops_sig_type_t		type;		/*!< signature type value */
    time_t			creation_time;	/*!< creation time of the signature - only with v3 signatures*/
    unsigned char		signer_id[OPS_KEY_ID_SIZE];	/*!< Eight-octet key ID of signer*/
    ops_public_key_algorithm_t	key_algorithm;	/*!< public key algorithm number */
    ops_hash_algorithm_t	hash_algorithm;	/*!< hashing algorithm number */
    unsigned char		hash2[2];	/*!< high 2 bytes of hashed value - for quick test */
    ops_signature_union_t	signature;	/*!< signature parameters */
    size_t			v4_hashed_data_start; // only valid if accumulate is set
    size_t			v4_hashed_data_length;
    } ops_signature_t;

/** The raw bytes of a signature subpacket */

typedef struct
    {
    ops_content_tag_t		tag;
    size_t			length;
    unsigned char		*raw;
    } ops_ss_raw_t;

/** Signature Subpacket Type 5, Trust Level */

typedef struct
    {
    unsigned char		level;	/*<! Trust Level */
    unsigned char		amount; /*<! Amount */
    } ops_ss_trust_t;

typedef struct
	{
	ops_boolean_t	revocable;
	} ops_ss_revocable_t;
	
typedef struct
    {
    time_t			time;
    } ops_ss_time_t;

typedef struct
    {
    unsigned char		key_id[OPS_KEY_ID_SIZE];
    } ops_ss_key_id_t;

/** SS Notation Data */
typedef struct
    {
    ops_data_t flags;
    ops_data_t name;
    ops_data_t value;
    } ops_ss_notation_data_t;

typedef struct
    {
    ops_data_t data;
    } ops_ss_userdefined_t;

typedef struct
    {
    ops_data_t data;
    } ops_ss_unknown_t;

/** SS Preferred Symmetric Key Algorithm */
typedef struct
    {
    ops_data_t data;
    /* Note that value 0 may represent the plaintext algorithm
       so we cannot expect data->contents to be a null-terminated list */
    } ops_ss_preferred_ska_t;

/** SS Preferrred Hash Algorithm */
typedef struct
    {
    ops_data_t data;
    } ops_ss_preferred_hash_t;

/** SS Preferred Compression */
typedef struct
    {
    ops_data_t data;
    } ops_ss_preferred_compression_t;

/** SS Key Flags */
typedef struct
    {
    ops_data_t data;
    } ops_ss_key_flags_t;

/** SS Key Server Preferences */
typedef struct
    {
    ops_data_t data;
    } ops_ss_key_server_prefs_t;

/** SS Features */
typedef struct
    {
    ops_data_t data;
    } ops_ss_features_t;

typedef struct
    {
    size_t			length;
    unsigned char		*raw;
    } ops_packet_t;

typedef enum
    {
    OPS_C_NONE=0,
    OPS_C_ZIP=1,
    OPS_C_ZLIB=2,
    OPS_C_BZIP2=3,
    } ops_compression_type_t;

// unlike most structures, this will feed its data as a stream
// to the application instead of directly including it
typedef struct
    {
    ops_compression_type_t	type;
    } ops_compressed_t;

typedef struct
    {
    unsigned char		version;
    ops_sig_type_t		sig_type;
    ops_hash_algorithm_t	hash_algorithm;
    ops_public_key_algorithm_t	key_algorithm;
    unsigned char		keyid[OPS_KEY_ID_SIZE];
    ops_boolean_t		nested;
    } ops_one_pass_signature_t;

typedef struct
    {
    ops_boolean_t	primary_user_id;
    } ops_ss_primary_user_id_t;

typedef struct
    {
    char *text;
    } ops_ss_regexp_t;

typedef struct
    {
    char *text;
    } ops_ss_policy_url_t;

typedef struct
    {
    char *text;
    } ops_ss_preferred_key_server_t;

typedef struct
    {
    unsigned char	class;
    unsigned char	algid;
    unsigned char fingerprint[20];
    } ops_ss_revocation_key_t;

typedef struct
    {
    unsigned char code;
    char *text;
    } ops_ss_revocation_reason_t;

typedef enum
    {
    OPS_LDT_BINARY='b',
    OPS_LDT_TEXT='t',
    OPS_LDT_UTF8='u',
    OPS_LDT_LOCAL='l',
    OPS_LDT_LOCAL2='1'
    } literal_data_type_t;

typedef struct
    {
    literal_data_type_t		format;
    char			filename[256];
    time_t			modification_time;
    } ops_literal_data_header_t;

typedef struct
    {
    unsigned			length;
    unsigned char		data[8192];
    } ops_literal_data_body_t;

typedef struct
    {
    char *key;
    char *value;
    } ops_armoured_header_value_t;

typedef struct
    {
    ops_armoured_header_value_t *headers;
    unsigned nheaders;
    } ops_headers_t;

typedef struct
    {
    const char *type;
    ops_headers_t headers;
    } ops_armour_header_t;

typedef struct
    {
    const char *type;
    } ops_armour_trailer_t;

typedef struct
    {
    ops_headers_t headers;
    } ops_signed_cleartext_header_t;

typedef struct
    {
    unsigned			length;
    unsigned char		data[8192];
    } ops_signed_cleartext_body_t;

typedef struct
    {
    struct _ops_hash_t		*hash;	/*!< This will not have been finalised, but will have seen all the cleartext data in canonical form */
    } ops_signed_cleartext_trailer_t;

typedef struct
    {
    unsigned			length;
    unsigned char		*data;
    } ops_unarmoured_text_t;

typedef union
    {
    ops_parser_error_t		error;
    ops_parser_errcode_t	errcode;
    ops_ptag_t			ptag;
    ops_public_key_t		public_key;
    ops_trust_t			trust;
    ops_user_id_t		user_id;
    ops_user_attribute_t	user_attribute;
    ops_signature_t		signature;
    ops_ss_raw_t		ss_raw;
    ops_ss_trust_t		ss_trust;
    ops_ss_revocable_t		ss_revocable;
    ops_ss_time_t		ss_time;
    ops_ss_key_id_t		ss_issuer_key_id;
    ops_ss_notation_data_t	ss_notation_data;
    ops_packet_t		packet;
    ops_compressed_t		compressed;
    ops_one_pass_signature_t	one_pass_signature;
    ops_ss_preferred_ska_t	ss_preferred_ska;
    ops_ss_preferred_hash_t     ss_preferred_hash;
    ops_ss_preferred_compression_t     ss_preferred_compression;
    ops_ss_key_flags_t 		ss_key_flags;
    ops_ss_key_server_prefs_t	ss_key_server_prefs;
    ops_ss_primary_user_id_t	ss_primary_user_id;
    ops_ss_regexp_t		ss_regexp;
    ops_ss_policy_url_t 	ss_policy_url;
    ops_ss_preferred_key_server_t	ss_preferred_key_server;
    ops_ss_revocation_key_t	ss_revocation_key;
    ops_ss_userdefined_t 	ss_userdefined;
    ops_ss_unknown_t	 	ss_unknown;
    ops_literal_data_header_t	literal_data_header;
    ops_literal_data_body_t	literal_data_body;
    ops_ss_features_t		ss_features;
    ops_ss_revocation_reason_t	ss_revocation_reason;
    ops_secret_key_t		secret_key;
    ops_user_id_t		ss_signers_user_id;
    ops_armour_header_t		armour_header;
    ops_armour_trailer_t	armour_trailer;
    ops_signed_cleartext_header_t signed_cleartext_header;
    ops_signed_cleartext_body_t	signed_cleartext_body;
    ops_signed_cleartext_trailer_t signed_cleartext_trailer;
    ops_unarmoured_text_t	unarmoured_text;
    } ops_parser_content_union_t;

struct ops_parser_content_t
    {
    ops_content_tag_t		tag;
    unsigned char		critical; /* for signature subpackets */
    ops_parser_content_union_t 	content;
    };

typedef struct
    {
    unsigned char 		fingerprint[20];
    unsigned			length;
    } ops_fingerprint_t;

void ops_init(void);
void ops_finish(void);
void ops_keyid(unsigned char keyid[OPS_KEY_ID_SIZE],
	       const ops_public_key_t *key);
void ops_fingerprint(ops_fingerprint_t *fp,const ops_public_key_t *key);
void ops_public_key_free(ops_public_key_t *key);
void ops_user_id_free(ops_user_id_t *id);
void ops_user_attribute_free(ops_user_attribute_t *att);
void ops_signature_free(ops_signature_t *sig);
void ops_trust_free(ops_trust_t *trust);
void ops_ss_preferred_ska_free(ops_ss_preferred_ska_t *ss_preferred_ska);
void ops_ss_preferred_hash_free(ops_ss_preferred_hash_t *ss_preferred_hash);
void ops_ss_preferred_compression_free(ops_ss_preferred_compression_t *ss_preferred_compression);
void ops_ss_key_flags_free(ops_ss_key_flags_t *ss_key_flags);
void ops_ss_key_server_prefs_free(ops_ss_key_server_prefs_t *ss_key_server_prefs);
void ops_ss_features_free(ops_ss_features_t *ss_features);
void ops_ss_notation_data_free(ops_ss_notation_data_t *ss_notation_data);
void ops_ss_policy_url_free(ops_ss_policy_url_t *ss_policy_url);
void ops_ss_preferred_key_server_free(ops_ss_preferred_key_server_t *ss_preferred_key_server);
void ops_ss_regexp_free(ops_ss_regexp_t *ss_regexp);
void ops_ss_userdefined_free(ops_ss_userdefined_t *ss_userdefined);
void ops_ss_reserved_free(ops_ss_unknown_t *ss_unknown);
void ops_ss_revocation_reason_free(ops_ss_revocation_reason_t *ss_revocation_reason);
void ops_packet_free(ops_packet_t *packet);
void ops_parser_content_free(ops_parser_content_t *c);
void ops_secret_key_free(ops_secret_key_t *key);

/* vim:set textwidth=120: */
/* vim:set ts=8: */

#endif