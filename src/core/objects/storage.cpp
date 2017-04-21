#include "storage.h"

Storage::Storage()
{
	this->handle = reinterpret_cast<CK_OBJECT_HANDLE>(this);
	this->token = false;
	this->_private = false;
	this->modifiable = false;
	this->copyable = false;
	this->label = Scoped<std::string>(new std::string());
}


Storage::~Storage()
{
}

CK_RV Storage::GetAttributeValue
(
	CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes; gets values */
	CK_ULONG          ulCount     /* attributes in template */
)
{
	CHECK_ARGUMENT_NULL(pTemplate);
	CK_RV res = CKR_OK;

	for (size_t i = 0; i < ulCount && res == CKR_OK; i++) {
		CK_ATTRIBUTE_PTR attr = &pTemplate[i];

		switch (attr->type) {
		case CKA_TOKEN:
			res = GetToken((CK_BYTE_PTR)attr->pValue, &attr->ulValueLen);
			break;
		case CKA_PRIVATE:
			res = GetPrivate((CK_BYTE_PTR)attr->pValue, &attr->ulValueLen);
			break;
		case CKA_MODIFIABLE:
			res = GetModifiable((CK_BYTE_PTR)attr->pValue, &attr->ulValueLen);
			break;
		case CKA_LABEL:
			res = GetLabel((CK_BYTE_PTR)attr->pValue, &attr->ulValueLen);
			break;
		case CKA_COPYABLE:
			res = GetCopyable((CK_BYTE_PTR)attr->pValue, &attr->ulValueLen);
			break;
		default:
			res = Object::GetAttributeValue(attr, 1);
		}
	}

	return res;
}

CK_RV Storage::SetAttributeValue
(
	CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes and values */
	CK_ULONG          ulCount     /* attributes in template */
)
{
	return Object::GetAttributeValue(pTemplate, ulCount);
}

DECLARE_GET_ATTRIBUTE(Storage::GetToken)
{
	return this->GetBool(pValue, pulValueLen, token);
}

DECLARE_GET_ATTRIBUTE(Storage::GetPrivate)
{
	return this->GetBool(pValue, pulValueLen, _private);
}

DECLARE_GET_ATTRIBUTE(Storage::GetModifiable)
{
	return this->GetBool(pValue, pulValueLen, modifiable);
}

DECLARE_GET_ATTRIBUTE(Storage::GetLabel)
{
	return this->GetUtf8String(pValue, pulValueLen, (BYTE*)label->c_str(), label->length());
}

DECLARE_GET_ATTRIBUTE(Storage::GetCopyable)
{
	return this->GetBool(pValue, pulValueLen, copyable);
}
