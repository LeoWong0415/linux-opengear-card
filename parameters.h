/* 
 * File:   parameters.h
 * Author: sau
 *
 * Created on August 29, 2012, 9:28 AM
 */

#ifndef PARAMETERS_H
#define	PARAMETERS_H
#include "ogp.h"
#include "types1.h"


void OGP_StartProcessLongData(OgpLongFrame * asData);
void OGP_GetParamRespone(unsigned char dest,ObjectID oid);
void OGP_SendInt16ParamValue(unsigned char dest, Int16Param* param);
void OGP_SendFloatParamValue(unsigned char dest, FloatParam* param);
void OGP_SendStringParamValue(unsigned char dest,StringParam* param);
void OGP_SendGetParamNotFound(unsigned char dest, ObjectID oid);
void OGP_SendInt16ArrParamValue(unsigned char dest,Int16ArrayParam* param);
void OGP_SendStrArrParamValue(unsigned char dest,StrArrayParam* param);
void OGP_SendInt32ParamValue(unsigned char dest, Int32Param* param);
void OGP_SendInt16ArrParamDesc(unsigned char dest, Int16ArrayParam* param);
void OGP_GetNumOfParamRespone(unsigned char dest);
void OGP_GetParamOidsRespone(unsigned char dest,unsigned char index,unsigned char nParam);
void OGP_GetDescriptorRespone(unsigned char dest,ObjectID oid);
void OGP_SendTabParamDesc(unsigned char dest, StringParam* param);
void OGP_SendInt16ParamDesc(unsigned char dest, Int16Param* param);
void OGP_SendInt32ParamDesc(unsigned char dest, Int32Param* param);
void OGP_SendFloatParamDesc(unsigned char dest, FloatParam* param);
void OGP_SendStringParamDesc(unsigned char dest, StringParam* param);
void OGP_SendStrArrParamDesc(unsigned char dest, StrArrayParam* param);
void OGP_SendWidgetStringParamDesc(unsigned char dest, StringParam* param,unsigned char widget);
void OGP_SendParamNotFound(unsigned char dest, ObjectID oid);
void OGP_SetInt16ParamRespone(unsigned char dest, Int16Param* param);
void OGP_SetInt32ParamRespone(unsigned char dest, Int32Param* param);
void OGP_SetInt16ArrayParamRespone(unsigned char dest, Int16ArrayParam* param);
void OGP_SetFloatParamRespone(BYTE dest,FloatParam * param);
void OGP_SetStringParamRespone(unsigned char dest,UINT16 oid,char* data,unsigned char len);
void OGP_SetArrayElementRespone(OgpShortFrame * pshort);
void OGP_RebootRespone(BYTE dest);
void OGP_VerifyUploadRespone(BYTE dest,BYTE type);
void OGP_GetSnmpBaseOidRespone(BYTE dest);
void OGP_SendInt16ArrayElementValue(BYTE dest,UINT16 oid,UINT16 index);
void OGP_SetArrayElementRespone(OgpShortFrame * pshort);
void OGP_ReportInt16Param(Int16Param* param);
void OGP_ReportInt32Param(Int32Param* param);
void OGP_ReportFloatParam(FloatParam* param);
void OGP_ReportStringParam(StringParam* param);
void OGP_ReportObjectParam(ObjectID * pOid);
void OGP_ReportInt16ArrayParam(Int16ArrayParam* param);
void OGP_CyclicReportParams(ReportObject * robjs,int size);
void ReportGroupParamsChanged(ObjectID * grp, int size);
void LoadDefaultValues(short mode);
void UpdateUserDefineValue(void);

void ObjectEnable(ObjectID oid, unsigned char val);
void GroupEnable(ObjectID * objs, int size, unsigned char val);

void CheckVariablesChanged(void);
void ChangeObjectDesc(ObjectID oid, char enable);
void ChangeGroupDesc(ObjectID * objs, int size, unsigned char val);

#endif	/* PARAMETERS_H */

