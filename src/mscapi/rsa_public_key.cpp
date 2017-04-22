#include "rsa_public_key.h"
#include "helper.h"

MscapiRsaPublicKey::MscapiRsaPublicKey(Scoped<crypt::Key> key, CK_BBOOL token) :
	value(key)
{
	this->token = token;
	*this->label = "RSA public key";
}

MscapiRsaPublicKey::~MscapiRsaPublicKey()
{
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetID)
{
	return this->GetBytes(pValue, pulValueLen, (CK_BYTE_PTR)this->id.c_str(), this->id.length());
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetStartDate)
{
	return this->GetBytes(pValue, pulValueLen, NULL, 0);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetEndDate)
{
	return this->GetBytes(pValue, pulValueLen, NULL, 0);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetDerive)
{
	return this->GetBool(pValue, pulValueLen, CK_FALSE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetLocal)
{
	return this->GetBool(pValue, pulValueLen, CK_FALSE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetKeyGenMechanism)
{
	return this->GetNumber(pValue, pulValueLen, CKM_RSA_PKCS);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetAllowedMechanisms)
{
	return this->GetBytes(pValue, pulValueLen, NULL, 0);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetSubject)
{
	return this->GetBytes(pValue, pulValueLen, NULL, 0);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetEncrypt)
{
	return this->GetBool(pValue, pulValueLen, this->value->GetProvider()->GetKeySpec() & AT_KEYEXCHANGE == AT_KEYEXCHANGE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetVerify)
{
	fprintf(stdout, "KeySpec: %d\n", this->value->GetProvider()->GetKeySpec());
	return this->GetBool(pValue, pulValueLen, this->value->GetProvider()->GetKeySpec() & AT_SIGNATURE == AT_SIGNATURE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetVerifyRecover)
{
	return this->GetBool(pValue, pulValueLen, CK_FALSE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetWrap)
{
	return this->GetBool(pValue, pulValueLen, this->value->GetProvider()->GetKeySpec() & AT_KEYEXCHANGE == AT_KEYEXCHANGE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetTrusted)
{
	return this->GetBool(pValue, pulValueLen, CK_FALSE);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetWrapTemplate)
{
	return this->GetBytes(pValue, pulValueLen, NULL, 0);
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetModulus)
{
	DWORD dwPublicKeyLen = 0;
	BYTE* pbPublicKey = NULL;

	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLen)) {
		THROW_MS_ERROR();
	}
	pbPublicKey = (BYTE*)malloc(dwPublicKeyLen);
	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, pbPublicKey, &dwPublicKeyLen)) {
		THROW_MS_ERROR();
	}

	PUBLICKEYSTRUC* header = (PUBLICKEYSTRUC*)pbPublicKey;
	RSAPUBKEY* info = (RSAPUBKEY*)(pbPublicKey + sizeof(PUBLICKEYSTRUC));
	BYTE* modulus = (BYTE*)(pbPublicKey + sizeof(PUBLICKEYSTRUC) + sizeof(RSAPUBKEY));

	// reverse bytes
	std::reverse(&modulus[0], &modulus[info->bitlen / 8]);

	CK_RV rv = this->GetBytes(pValue, pulValueLen, modulus, info->bitlen / 8);

	free(pbPublicKey);

	return rv;
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetModulusBits)
{
	DWORD dwPublicKeyLen = 0;
	BYTE* pbPublicKey = NULL;

	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLen)) {
		THROW_MS_ERROR();
	}
	pbPublicKey = (BYTE*)malloc(dwPublicKeyLen);
	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, pbPublicKey, &dwPublicKeyLen)) {
		free(pbPublicKey);
		THROW_MS_ERROR();
	}

	PUBLICKEYSTRUC* header = (PUBLICKEYSTRUC*)pbPublicKey;
	RSAPUBKEY* info = (RSAPUBKEY*)(pbPublicKey + sizeof(PUBLICKEYSTRUC));

	CK_RV rv = this->GetNumber(pValue, pulValueLen, info->bitlen);

	free(pbPublicKey);

	return rv;
}

DECLARE_GET_ATTRIBUTE(MscapiRsaPublicKey::GetPublicExponent)
{
	DWORD dwPublicKeyLen = 0;
	BYTE* pbPublicKey = NULL;

	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLen)) {
		THROW_MS_ERROR();
	}
	pbPublicKey = (BYTE*)malloc(dwPublicKeyLen);
	if (!CryptExportKey(this->value->Get(), NULL, PUBLICKEYBLOB, 0, pbPublicKey, &dwPublicKeyLen)) {
		free(pbPublicKey);
		THROW_MS_ERROR();
	}

	PUBLICKEYSTRUC* header = (PUBLICKEYSTRUC*)pbPublicKey;
	RSAPUBKEY* info = (RSAPUBKEY*)(pbPublicKey + sizeof(PUBLICKEYSTRUC));

	CK_BYTE_PTR pbPublicExponent;
	if (info->pubexp == 2) {
		pbPublicExponent = (CK_BYTE_PTR)malloc(1);
		pbPublicExponent[0] = 3;
	}
	else {
		pbPublicExponent = (CK_BYTE_PTR)malloc(3);
		pbPublicExponent[0] = 1;
		pbPublicExponent[1] = 0;
		pbPublicExponent[2] = 1;
	}
	CK_RV rv = this->GetBytes(pValue, pulValueLen, pbPublicExponent, info->pubexp == 3 ? 1 : 3);

	free(pbPublicKey);
	free(pbPublicExponent);

	return rv;
}