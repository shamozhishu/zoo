/*******************************************************************************
 * Project:  neb
 * @file     JsonObject.cpp
 * @brief 
 * @author   bwarliao
 * @date:    2014-7-16
 * @note
 * Modify history:
 ******************************************************************************/

#include "JsonObject.h"

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

JsonObject::JsonObject()
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
{
}

JsonObject::JsonObject(const std::string& strJson)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
{
    Parse(strJson);
}

JsonObject::JsonObject(const JsonObject* pJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
{
    if (pJsonObject)
    {
        Parse(pJsonObject->ToString());
    }
}

JsonObject::JsonObject(const JsonObject& oJsonObject)
    : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
{
    Parse(oJsonObject.ToString());
}

JsonObject::~JsonObject()
{
    Clear();
}

JsonObject& JsonObject::operator=(const JsonObject& oJsonObject)
{
    Parse(oJsonObject.ToString().c_str());
    return(*this);
}

bool JsonObject::operator==(const JsonObject& oJsonObject) const
{
    return(this->ToString() == oJsonObject.ToString());
}

bool JsonObject::AddEmptySubObject(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateObject();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty object error!");
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::AddEmptySubArray(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateArray();
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("create sub empty array error!");
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::GetKey(std::string& strKey)
{
    if (IsArray())
    {
        return(false);
    }
    if (m_pKeyTravers == NULL)
    {
        if (m_pJsonData != NULL)
        {
            m_pKeyTravers = m_pJsonData;
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            m_pKeyTravers = m_pExternJsonDataRef;
        }
        return(false);
    }
    else if (m_pKeyTravers == m_pJsonData || m_pKeyTravers == m_pExternJsonDataRef)
    {
        cJSON *c = m_pKeyTravers->child;
        if (c)
        {
            strKey = c->string;
            m_pKeyTravers = c->next;
            return(true);
        }
        else
        {
            return(false);
        }
    }
    else
    {
        strKey = m_pKeyTravers->string;
        m_pKeyTravers = m_pKeyTravers->next;
        return(true);
    }
}

void JsonObject::ResetTraversing()
{
    if (m_pJsonData != NULL)
    {
        m_pKeyTravers = m_pJsonData;
    }
    else
    {
        m_pKeyTravers = m_pExternJsonDataRef;
    }
}

JsonObject& JsonObject::operator[](const std::string& strKey)
{
    std::map<std::string, JsonObject*>::iterator iter;
    iter = m_mapJsonObjectRef.find(strKey);
    if (iter == m_mapJsonObjectRef.end())
    {
        cJSON* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSON_Object)
            {
                pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSON_Object)
            {
                pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
            }
        }
        if (pJsonStruct == NULL)
        {
            JsonObject* pJsonObject = new JsonObject();
            m_mapJsonObjectRef.insert(std::pair<std::string, JsonObject*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            JsonObject* pJsonObject = new JsonObject(pJsonStruct);
            m_mapJsonObjectRef.insert(std::pair<std::string, JsonObject*>(strKey, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

JsonObject& JsonObject::operator[](unsigned int uiWhich)
{
    std::map<unsigned int, JsonObject*>::iterator iter;
    iter = m_mapJsonArrayRef.find(uiWhich);
    if (iter == m_mapJsonArrayRef.end())
    {
        cJSON* pJsonStruct = NULL;
        if (m_pJsonData != NULL)
        {
            if (m_pJsonData->type == cJSON_Array)
            {
                pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
            }
        }
        else if (m_pExternJsonDataRef != NULL)
        {
            if (m_pExternJsonDataRef->type == cJSON_Array)
            {
                pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
            }
        }
        if (pJsonStruct == NULL)
        {
            JsonObject* pJsonObject = new JsonObject();
            m_mapJsonArrayRef.insert(std::pair<unsigned int, JsonObject*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
        else
        {
            JsonObject* pJsonObject = new JsonObject(pJsonStruct);
            m_mapJsonArrayRef.insert(std::pair<unsigned int, JsonObject*>(uiWhich, pJsonObject));
            return(*pJsonObject);
        }
    }
    else
    {
        return(*(iter->second));
    }
}

std::string JsonObject::operator()(const std::string& strKey) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSON_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if (pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if ((uint64)pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSON_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSON_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

std::string JsonObject::operator()(unsigned int uiWhich) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(std::string(""));
    }
    if (pJsonStruct->type == cJSON_String)
    {
        return(pJsonStruct->valuestring);
    }
    else if (pJsonStruct->type == cJSON_Int)
    {
        char szNumber[128] = {0};
        if (pJsonStruct->sign == -1)
        {
            if (pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
            {
                snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
            }
        }
        else
        {
            if ((uint64)pJsonStruct->valueint <= (uint64)UINT_MAX)
            {
                snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
            }
            else
            {
                snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
            }
        }
        return(std::string(szNumber));
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        char szNumber[128] = {0};
        if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
        {
            snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
        }
        else
        {
            snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
        }
    }
    else if (pJsonStruct->type == cJSON_False)
    {
        return(std::string("false"));
    }
    else if (pJsonStruct->type == cJSON_True)
    {
        return(std::string("true"));
    }
    return(std::string(""));
}

bool JsonObject::Parse(const std::string& strJson)
{
    Clear();
    m_pJsonData = cJSON_Parse(strJson.c_str());
    m_pKeyTravers = m_pJsonData;
    if (m_pJsonData == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    return(true);
}

void JsonObject::Clear()
{
    m_pExternJsonDataRef = NULL;
    m_pKeyTravers = NULL;
    if (m_pJsonData != NULL)
    {
        cJSON_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
    for (std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonArrayRef.clear();
    for (std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.begin();
                    iter != m_mapJsonObjectRef.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
    }
    m_mapJsonObjectRef.clear();
}

bool JsonObject::IsEmpty() const
{
    if (m_pJsonData != NULL)
    {
        return(false);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::IsArray() const
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }

    if (pFocusData == NULL)
    {
        return(false);
    }

    if (pFocusData->type == cJSON_Array)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

std::string JsonObject::ToString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSON_PrintUnformatted(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSON_PrintUnformatted(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}

std::string JsonObject::ToFormattedString() const
{
    char* pJsonString = NULL;
    std::string strJsonData = "";
    if (m_pJsonData != NULL)
    {
        pJsonString = cJSON_Print(m_pJsonData);
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pJsonString = cJSON_Print(m_pExternJsonDataRef);
    }
    if (pJsonString != NULL)
    {
        strJsonData = pJsonString;
        free(pJsonString);
    }
    return(strJsonData);
}


bool JsonObject::Get(const std::string& strKey, JsonObject& oJsonObject) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSON_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool JsonObject::Get(const std::string& strKey, std::string& strValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool JsonObject::Get(const std::string& strKey, int32& iValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        iValue = (int32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        iValue = (int32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(const std::string& strKey, uint32& uiValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        uiValue = (uint32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        uiValue = (uint32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(const std::string& strKey, int64& llValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        llValue = (int64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        llValue = (int64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(const std::string& strKey, uint64& ullValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        ullValue = (uint64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        ullValue = (uint64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(const std::string& strKey, bool& bValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSON_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type;
    return(true);
}

bool JsonObject::Get(const std::string& strKey, float& fValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
    {
        fValue = (float)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(const std::string& strKey, double& dValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
    {
        dValue = pJsonStruct->valuedouble;
        return(true);
    }
    return(false);
}

bool JsonObject::IsNull(const std::string& strKey) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Object)
        {
            pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(const std::string& strKey, const JsonObject& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt(ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Add(const std::string& strKey, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::AddNull(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateObject();
        m_pKeyTravers = m_pJsonData;
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
    {
        m_strErrMsg = "key exists!";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Delete(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON_DeleteItemFromObject(pFocusData, strKey.c_str());
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    m_pKeyTravers = pFocusData;
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, const JsonObject& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(const std::string& strKey, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::ReplaceWithNull(const std::string& strKey)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Object)
    {
        m_strErrMsg = "not a json object! json array?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<std::string, JsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
    if (iter != m_mapJsonObjectRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonObjectRef.erase(iter);
    }
    cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
    if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
    {
        return(false);
    }
    return(true);
}

int JsonObject::GetArraySize()
{
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            return(cJSON_GetArraySize(m_pJsonData));
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            return(cJSON_GetArraySize(m_pExternJsonDataRef));
        }
    }
    return(0);
}

bool JsonObject::Get(int iWhich, JsonObject& oJsonObject) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    char* pJsonString = cJSON_Print(pJsonStruct);
    std::string strJsonData = pJsonString;
    free(pJsonString);
    if (oJsonObject.Parse(strJsonData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool JsonObject::Get(int iWhich, std::string& strValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_String)
    {
        return(false);
    }
    strValue = pJsonStruct->valuestring;
    return(true);
}

bool JsonObject::Get(int iWhich, int32& iValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        iValue = (int32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        iValue = (int32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(int iWhich, uint32& uiValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        uiValue = (uint32)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        uiValue = (uint32)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(int iWhich, int64& llValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        llValue = (int64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        llValue = (int64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(int iWhich, uint64& ullValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Int)
    {
        ullValue = (uint64)(pJsonStruct->valueint);
        return(true);
    }
    else if (pJsonStruct->type == cJSON_Double)
    {
        ullValue = (uint64)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(int iWhich, bool& bValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type > cJSON_True)
    {
        return(false);
    }
    bValue = pJsonStruct->type;
    return(true);
}

bool JsonObject::Get(int iWhich, float& fValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
    {
        fValue = (float)(pJsonStruct->valuedouble);
        return(true);
    }
    return(false);
}

bool JsonObject::Get(int iWhich, double& dValue) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
    {
        dValue = pJsonStruct->valuedouble;
        return(true);
    }
    return(false);
}

bool JsonObject::IsNull(int iWhich) const
{
    cJSON* pJsonStruct = NULL;
    if (m_pJsonData != NULL)
    {
        if (m_pJsonData->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
        }
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        if(m_pExternJsonDataRef->type == cJSON_Array)
        {
            pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
        }
    }
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    if (pJsonStruct->type != cJSON_NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(const JsonObject& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    unsigned int uiLastIndex = (unsigned int)cJSON_GetArraySize(pFocusData) - 1;
    for (std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= uiLastIndex)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool JsonObject::Add(const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(int iAnywhere, bool bValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Add(double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddNull()
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArray(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(const JsonObject& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    for (std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter++);
    }
    return(true);
}

bool JsonObject::AddAsFirst(const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(int iAnywhere, bool bValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddAsFirst(double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::AddNullAsFirst()
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData != NULL)
    {
        pFocusData = m_pJsonData;
    }
    else if (m_pExternJsonDataRef != NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        m_pJsonData = cJSON_CreateArray();
        pFocusData = m_pJsonData;
    }

    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
    cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
    int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
    if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Delete(int iWhich)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON_DeleteItemFromArray(pFocusData, iWhich);
    for (std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
                    iter != m_mapJsonArrayRef.end(); )
    {
        if (iter->first >= (unsigned int)iWhich)
        {
            if (iter->second != NULL)
            {
                delete (iter->second);
                iter->second = NULL;
            }
            m_mapJsonArrayRef.erase(iter++);
        }
        else
        {
            iter++;
        }
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, const JsonObject& oJsonObject)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str());
    if (pJsonStruct == NULL)
    {
        m_strErrMsg = std::string("prase json string error at ") + cJSON_GetErrorPtr();
        return(false);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, const std::string& strValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, int32 iValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, uint32 uiValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, int64 llValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)((uint64)llValue), -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, uint64 ullValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, bool bValue, bool bValueAgain)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateBool(bValue);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, float fValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::Replace(int iWhich, double dValue)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

bool JsonObject::ReplaceWithNull(int iWhich)
{
    cJSON* pFocusData = NULL;
    if (m_pJsonData == NULL)
    {
        pFocusData = m_pExternJsonDataRef;
    }
    else
    {
        pFocusData = m_pJsonData;
    }
    if (pFocusData == NULL)
    {
        m_strErrMsg = "json data is null!";
        return(false);
    }
    if (pFocusData->type != cJSON_Array)
    {
        m_strErrMsg = "not a json array! json object?";
        return(false);
    }
    cJSON* pJsonStruct = cJSON_CreateNull();
    if (pJsonStruct == NULL)
    {
        return(false);
    }
    std::map<unsigned int, JsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
    if (iter != m_mapJsonArrayRef.end())
    {
        if (iter->second != NULL)
        {
            delete (iter->second);
            iter->second = NULL;
        }
        m_mapJsonArrayRef.erase(iter);
    }
    cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
    if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
    {
        return(false);
    }
    return(true);
}

JsonObject::JsonObject(cJSON* pJsonData)
    : m_pJsonData(NULL), m_pExternJsonDataRef(pJsonData), m_pKeyTravers(pJsonData)
{
}


