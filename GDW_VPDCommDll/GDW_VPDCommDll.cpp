// GDW_VPDCommDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GDW_VPDCommDll.h"
#include "Camera6467_plate.h"

extern void g_WriteLog(const char* chLog);
extern void g_ReadKeyValueFromConfigFile(const char* nodeName, const char* keyName, char* keyValue, int bufferSize);

extern Camera6467_plate* g_pCamera6467;
//CameraIMG g_IMG_Temp;

#include "ToolFunction.h"


GDW_VPDCOMMDLL_API int WINAPI GDW_Connect(HWND hWnd, UINT uiDataMsgNo /*= 0x401*/, UINT uiReConnectMsg /*= 0x402*/, UINT uiDisconnectMsg /*= 0x403*/)
{
    g_WriteLog("GDW_Connect  begin.");
    if (g_pCamera6467 != NULL)
    {
        delete g_pCamera6467;
        g_pCamera6467 = NULL;
    }
    char chIP[256] = { 0 };
    g_ReadKeyValueFromConfigFile("Device", "IP", chIP, 256);
    g_WriteLog(chIP);
    g_pCamera6467 = new Camera6467_plate(chIP, hWnd, uiDataMsgNo);
    //g_Camera = new Camera6467(chIP, hWnd, uiDataMsgNo);
    if (!g_pCamera6467)
    {
        g_WriteLog("GDW_Connect  end and return 1.");
        return 1;
    }
    g_pCamera6467->SetMsg(uiReConnectMsg, uiDisconnectMsg);
    int iConnet = g_pCamera6467->ConnectToCamera();
    int iRetCode = -1;
    if (iConnet == 0)
    {
        g_WriteLog("GDW_Connect  end and return 0.");
        //::PostMessage(hWnd, uiReConnectMsg, NULL, NULL);
        iRetCode = 0;
    }
    else if (iConnet == -2)	//net not ready
    {
        g_WriteLog("GDW_Connect  end and return 4.");
        iRetCode = 4;
    }
    else if (iConnet == -3)
    {
        g_WriteLog("GDW_Connect  end and return 4.");
        iRetCode = 7;
    }
    else
    {
        iRetCode = 4;
        //::PostMessage(hWnd, uiDisconnectMsg, NULL, NULL);
        g_WriteLog("GDW_Connect  end and else return 4.");
    }
    if (iRetCode != 0)
    {
        if (g_pCamera6467 != NULL)
        {
            delete g_pCamera6467;
            g_pCamera6467 = NULL;
        }
    }

    return iRetCode;
}

GDW_VPDCOMMDLL_API int WINAPI GDW_Disconnect()
{
    g_WriteLog("GDW_Disconnect  begin.");
    if (g_pCamera6467 != NULL)
    {
        delete g_pCamera6467;
        g_pCamera6467 = NULL;
    }
    g_WriteLog("GDW_Disconnect  end.");
    return 0;
}

GDW_VPDCOMMDLL_API int WINAPI GDW_Capture()
{
    g_WriteLog("GDW_Capture  begin.");
    if (g_pCamera6467 == NULL)
    {
        g_WriteLog("GDW_Capture  end and return 1.");
        return 1;
    }
    bool bWaitSucess = false;
    bool bCapture = g_pCamera6467->TakeCapture();
    if (bCapture)
    {
        g_WriteLog("GDW_Capture  end and return 0.");

        int iTryTime = 30;
        while (iTryTime--)
        {
            if (g_pCamera6467->GetResultComplete())
            {
                g_WriteLog("wait result success.");
                bWaitSucess = true;
                break;
            }
            Sleep(50);
        }
    }
    else
    {
        g_WriteLog("GDW_Capture  end and return 4.");
    }  

    return (bWaitSucess ? 0 : 4);

    //return (bCapture ? 0 : 4);
}

GDW_VPDCOMMDLL_API int WINAPI GDW_GetVehicleInfo(char* pchPlate, char* pchTime, BYTE* pByteBinImage, BYTE* pBytePlateJpeg, UINT &dwPlateJpegSize, BYTE* pByteCarJpeg, UINT &dwCarJpegSize, BYTE* pByteFarJpeg, UINT &dwFarJpegSize)
{
    char chLog[256] = {0};
    g_WriteLog("GDW_GetVehicleInfo  begin.");
    if (g_pCamera6467 == NULL)
    {
        g_WriteLog("GDW_GetVehicleInfo:: the camera is not init, return 2.");
        return 2;
    }

    /*
    1、牌照识别系统传回车牌数据格式统一为”颜色+车辆牌照“，如”蓝晋A12345“，车牌颜色按部标准规定统一执行；
    2、无牌照或无法获取到车牌（如遮挡等），统一传回”*****“，不得有无数据传回现象。
    */
    
    CameraResult* pTempResult = g_pCamera6467->GetOneResult();
    if (!pTempResult)
    {
        g_WriteLog("GDW_GetVehicleInfo:: the camera result  is not ready, return 4, but return 0.");

        g_WriteLog("current result is 无车牌， use ' ***** ' instead.");
        memcpy(pchPlate, "*****", sizeof("*****"));
        //memcpy(pchPlate, "无车牌\0", sizeof("无车牌\0"));

        CameraIMG g_IMG_Temp;
        if (g_pCamera6467->GetOneJpegImg(g_IMG_Temp))
        {
            unsigned char* pTempBig1 = NULL;
            char chTemp[256] = { 0 };
            g_ReadKeyValueFromConfigFile("Overlay", "Enable", chTemp, 256);
            if (strstr(chTemp, "1"))
            {
                g_WriteLog("overlay...");
                if (pTempBig1 == NULL)
                {
                    pTempBig1 = new BYTE[MAX_IMG_SIZE];
                    memset(pTempBig1, 0, MAX_IMG_SIZE);
                }
                char chOverlayInfo[256] = { 0 };
                static time_t starttime = time(NULL);
                static DWORD starttick = GetTickCount();
                DWORD dwNowTick = GetTickCount() - starttick;
                time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
                //struct tm *pTM = localtime(&nowtime);
                struct tm pTM;
                localtime_s(&pTM, &nowtime);
                sprintf_s(chOverlayInfo, "时间: %04d%02d%02d %02d:%02d:%02d 牌号: *****", pTM.tm_year + 1900, pTM.tm_mon + 1, pTM.tm_mday,
                    pTM.tm_hour, pTM.tm_min, pTM.tm_sec);
                std::string strOverlayInfo(chOverlayInfo);
                std::wstring wstrOverlayIno = Img_string2wstring(strOverlayInfo);

                if (g_IMG_Temp.dwImgSize > 0 && g_IMG_Temp.pbImgData)
                {
                    if (pTempBig1)
                    {
                        memset(pTempBig1, 0, MAX_IMG_SIZE);
                        long iDestSize = MAX_IMG_SIZE;

                        memset(chLog, 0, sizeof(chLog));
                        MY_SPRINTF(chLog, "开始字符叠加， text = %s", chOverlayInfo);
                        g_WriteLog(chLog);

                        bool bOverlay = Tool_OverlayStringToImg(&g_IMG_Temp.pbImgData, g_IMG_Temp.dwImgSize,
                            &pTempBig1, iDestSize,
                            wstrOverlayIno.c_str(), 32,
                            10, 30, 255, 255, 255,
                            50);
                        if (bOverlay)
                        {
                            memset(chLog, 0, sizeof(chLog));
                            MY_SPRINTF(chLog, "字符叠加成功, size = %ld, begin copy..", iDestSize);
                            g_WriteLog(chLog);

                            delete[] g_IMG_Temp.pbImgData;
                            g_IMG_Temp.pbImgData = new BYTE[iDestSize];
                            memcpy(g_IMG_Temp.pbImgData, pTempBig1, iDestSize);
                            g_IMG_Temp.dwImgSize = iDestSize;

                            //FILE* pf = NULL;
                            //fopen_s(&pf, "./test.jpg", "wb+");
                            //if (pf)
                            //{
                            //    fwrite(g_IMG_Temp.pbImgData, 1, iDestSize, pf);
                            //    fclose(pf);
                            //    pf = NULL;
                            //}
                            printf("finish copy.");
                        }
                        else
                        {
                            printf("字符叠加失败,使用原图数据.");
                        }
                    }
                }
            }     

            g_WriteLog("get jpeg image  success, begin to compress image.");
            g_pCamera6467->CompressImg(g_IMG_Temp, BIG_IMG_SIZE);

            dwPlateJpegSize = 0;

            sprintf_s(chLog, "begin to copy car jpg, size = %ld", g_IMG_Temp.dwImgSize);
            g_WriteLog(chLog);
            memcpy(pByteCarJpeg, g_IMG_Temp.pbImgData, g_IMG_Temp.dwImgSize);
            dwCarJpegSize = g_IMG_Temp.dwImgSize;
            
            sprintf_s(chLog, "begin to copy far jpg, size = %ld", g_IMG_Temp.dwImgSize);
            g_WriteLog(chLog);
            memcpy(pByteFarJpeg, g_IMG_Temp.pbImgData, g_IMG_Temp.dwImgSize);
            dwFarJpegSize = g_IMG_Temp.dwImgSize;
            g_WriteLog("finish copy.");            
            
            if (pTempBig1 != NULL)
            {
                delete[] pTempBig1;
                pTempBig1 = NULL;
            }

        }
        else
        {
            g_WriteLog("get jpeg image failed.");
            dwPlateJpegSize = 0;
            dwCarJpegSize = 0;
            dwFarJpegSize = 0;
        }
        return 0;
    }
    g_pCamera6467->SetResultComplete(false);
    bool bOnlyPlate = false;
    //strcpy(pchPlate, pTempResult->chPlateNO);
    if (strstr(pTempResult->chPlateNO, "无"))
    {
        g_WriteLog("current result is 无车牌， use '*****' instead.");
        memcpy(pchPlate, "*****", sizeof("*****"));
    }
    else
    {
        strcpy_s(pchPlate, strlen(pTempResult->chPlateNO) + 1, pTempResult->chPlateNO);
    }
    
    g_WriteLog(pTempResult->chPlateNO);
    //strcpy(pchTime, pTempResult->chPlateTime);
    strcpy_s(pchTime, strlen(pTempResult->chPlateTime) + 1, pTempResult->chPlateTime);
    g_WriteLog(pTempResult->chPlateTime);

#ifdef  TEST

    if (pTempResult->CIMG_BinImage.dwImgSize > 0)
    {
        memcpy(pByteBinImage, pTempResult->CIMG_BinImage.pbImgData, pTempResult->CIMG_BinImage.dwImgSize);
        g_WriteLog("GDW_GetVehicleInfo:: get the binary Image.");
    }
    if (pTempResult->CIMG_PlateImage.dwImgSize > 0 && pTempResult->CIMG_PlateImage.dwImgSize <= PLATE_IMG_SIZE)
    {
        memcpy(pBytePlateJpeg, pTempResult->CIMG_PlateImage.pbImgData, pTempResult->CIMG_PlateImage.dwImgSize);
        dwPlateJpegSize = pTempResult->CIMG_PlateImage.dwImgSize;
        g_WriteLog("GDW_GetVehicleInfo:: get the plate Image.");
    }
    else
    {
        g_WriteLog("GDW_GetVehicleInfo:: plate Image 没有车牌图片或车牌图大于 5K.");
    }
    if (pTempResult->CIMG_BestSnapshot.dwImgSize > 0 && pTempResult->CIMG_BestSnapshot.dwImgSize <= BIG_IMG_SIZE)
    {
        memcpy(pByteFarJpeg, pTempResult->CIMG_BestSnapshot.pbImgData, pTempResult->CIMG_BestSnapshot.dwImgSize);
        dwFarJpegSize = pTempResult->CIMG_BestSnapshot.dwImgSize;
        g_WriteLog("GDW_GetVehicleInfo:: get the BestSnapshot Image.");
    }
    else
    {
        bOnlyPlate = true;
        g_WriteLog("GDW_GetVehicleInfo:: FarJpeg Image 没有车牌图片或车牌图大于 100K.");
    }
    if (pTempResult->CIMG_LastSnapshot.dwImgSize > 0 && pTempResult->CIMG_LastSnapshot.dwImgSize <= BIG_IMG_SIZE)
    {
        memcpy(pByteCarJpeg, pTempResult->CIMG_LastSnapshot.pbImgData, pTempResult->CIMG_LastSnapshot.dwImgSize);
        dwCarJpegSize = pTempResult->CIMG_LastSnapshot.dwImgSize;
        g_WriteLog("GDW_GetVehicleInfo:: get the LastSnapshot Image.");
    }
    else
    {
        bOnlyPlate = true;
        g_WriteLog("GDW_GetVehicleInfo:: CarJpeg Image 没有车牌图片或车牌图大于 100K.");
    }

#endif //  TEST

    if (pTempResult != NULL)
    {
        delete pTempResult;
        pTempResult = NULL;
    }

    if (bOnlyPlate)
    {
        g_WriteLog("GDW_GetVehicleInfo end and return 1.");
        return 1;
    }
    else
    {
        g_WriteLog("GDW_GetVehicleInfo end and return 0.");
        return 0;
    }
}

GDW_VPDCOMMDLL_API int WINAPI GDW_GetVehicleInfo2AD(char* pchPlate, char* pchTime, BYTE* pByteBinImage, BYTE* pBytePlateJpeg, UINT &dwPlateJpegSize, BYTE* pByteCarJpeg, UINT &dwCarJpegSize)
{
    g_WriteLog("GDW_GetVehicleInfo2AD  begin.");
    if (g_pCamera6467 == NULL)
    {
        g_WriteLog("GDW_GetVehicleInfo2AD:: the camera is not init, return 2.");
        return 2;
    }
    CameraResult* pTempResult = g_pCamera6467->GetOneResult();
    if (!pTempResult)
    {
        g_WriteLog("GDW_GetVehicleInfo:: the camera result  is not ready, return 4.");
        return 4;
    }
    g_pCamera6467->SetResultComplete(false);
    //strcpy(pchPlate, pTempResult->chPlateNO);
    strcpy_s(pchPlate, strlen(pTempResult->chPlateNO) + 1, pTempResult->chPlateNO);
    g_WriteLog(pTempResult->chPlateNO);
    //strcpy(pchTime, pTempResult->chPlateTime);
    strcpy_s(pchTime, strlen(pTempResult->chPlateTime) + 1, pTempResult->chPlateTime);
    g_WriteLog(pTempResult->chPlateTime);
    bool bOnlyPlate = false;
    if (pTempResult->CIMG_BinImage.dwImgSize > 0)
    {
        memcpy(pByteBinImage, pTempResult->CIMG_BinImage.pbImgData, pTempResult->CIMG_BinImage.dwImgSize);
        g_WriteLog("GDW_GetVehicleInfo2AD:: get the binary Image.");
    }
    if (pTempResult->CIMG_PlateImage.dwImgSize > 0 && pTempResult->CIMG_PlateImage.dwImgSize <= PLATE_IMG_SIZE)
    {
        memcpy(pBytePlateJpeg, pTempResult->CIMG_PlateImage.pbImgData, pTempResult->CIMG_PlateImage.dwImgSize);
        dwPlateJpegSize = pTempResult->CIMG_PlateImage.dwImgSize;
        g_WriteLog("GDW_GetVehicleInfo2AD:: get the plate Image.");
    }
    if (pTempResult->CIMG_LastSnapshot.dwImgSize > 0 && pTempResult->CIMG_LastSnapshot.dwImgSize <= BIG_IMG_SIZE)
    {
        memcpy(pByteCarJpeg, pTempResult->CIMG_LastSnapshot.pbImgData, pTempResult->CIMG_LastSnapshot.dwImgSize);
        dwCarJpegSize = pTempResult->CIMG_LastSnapshot.dwImgSize;
        g_WriteLog("GDW_GetVehicleInfo2AD:: get the LastSnapshot Image.");
    }
    else
    {
        bOnlyPlate = true;
        g_WriteLog("GDW_GetVehicleInfo2AD:: CarJpeg Image 没有车牌图片或车牌图大于 100K.");
    }

    if (pTempResult != NULL)
    {
        delete pTempResult;
        pTempResult = NULL;
    }

    if (bOnlyPlate)
    {
        g_WriteLog("GDW_GetVehicleInfo2AD end and return 1.");
        return 1;
    }
    else
    {
        g_WriteLog("GDW_GetVehicleInfo2AD end and return 0.");
        return 0;
    }
}

GDW_VPDCOMMDLL_API int WINAPI GDW_AdjustTime(char* pchTime)
{
    g_WriteLog("GDW_AdjustTime:: begin.");
    g_WriteLog(pchTime);
    if (g_pCamera6467 == NULL)
    {
        g_WriteLog("GDW_AdjustTime:: the camera is not init , return 1.");
        return 1;
    }
    if (strlen(pchTime) != 14)
    {
        //参数的长度不对
        g_WriteLog("GDW_AdjustTime:: the time string is invalid , return 2.");
        return 2;
    }
    int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMinute = 0, iSecond = 0;
    //sscanf(pchTime, "%4d%2d%2d%2d%2d%2d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
    sscanf_s(pchTime, "%4d%2d%2d%2d%2d%2d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);

    if (iYear > 9999 || iYear < 1000)
    {
        g_WriteLog("GDW_AdjustTime:: the year of time  is invalid , return 3.");
        return 3;
    }
    if (iMonth < 1 || iMonth > 12)
    {
        g_WriteLog("GDW_AdjustTime:: the month of time  is invalid , return 4.");
        return 4;
    }
    if (iDay < 1 || iDay > 31)
    {
        g_WriteLog("GDW_AdjustTime:: the day of time  is invalid , return 5.");
        return 5;
    }
    if (iHour < 0 || iHour > 23)
    {
        g_WriteLog("GDW_AdjustTime:: the hour of time  is invalid , return 6.");
        return 6;
    }
    if (iMinute < 0 || iMinute > 59)
    {
        g_WriteLog("GDW_AdjustTime:: the minute of time  is invalid , return 7.");
        return 7;
    }
    if (iSecond < 0 || iSecond > 59)
    {
        g_WriteLog("GDW_AdjustTime:: the second of time  is invalid , return 8.");
        return 8;
    }

    bool bSetTime = g_pCamera6467->SynTime(iYear, iMonth, iDay, iHour, iMinute, iSecond, 0);

    if (bSetTime)
    {
        g_WriteLog("GDW_AdjustTime:: end, syn time success , return 0.");
    }
    else
    {
        g_WriteLog("GDW_AdjustTime:: end, syn time failed , return 11.");
    }
    return (bSetTime ? 0 : 11);
}
