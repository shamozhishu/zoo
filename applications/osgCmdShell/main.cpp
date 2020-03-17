#include <osgCmd.h>
#include <stdio.h>
#include <thread>

static char* strtrim(char* s)
{
	char* p = s;
	char* q = s;
	while (*p == ' ' || *p == '\t' || *p == '\n') ++p;
	while (*q++ = *p++);
	q -= 2;
	while (*q == ' ' || *q == '\t' || *q == '\n') --q;
	*(q + 1) = '\0';
	return s;
}

static void parsecmdline()
{
	char szCmdline[128];
	while (true)
	{
		printf("--osgCmd: >> ");
		fgets(szCmdline, sizeof(szCmdline), stdin);
		if (feof(stdin))
			break;

		const char* pszCmdline = strtrim(szCmdline);
		if (0 == _stricmp(pszCmdline, "cls"))
		{
			system("cls");
			continue;
		}

		if (osgCmd_Send(pszCmdline))
		{
			if (0 == strcmp(pszCmdline, "--exit"))
				break;

			bool bRet;
			int iRet;
			float fRet;
			double dRet;

			if (nullptr != strstr(pszCmdline, "--test-bool") && osgCmd_BoolValue("test_bool", &bRet))
			{
				char szbuf[32];
				if (bRet)
					sprintf_s(szbuf, sizeof(szbuf), "true");
				else
					sprintf_s(szbuf, sizeof(szbuf), "false");
				printf("bool return value: %s\n", szbuf);
			}
			else if (nullptr != strstr(pszCmdline, "--test-int") && osgCmd_IntValue("test_int", &iRet))
				printf("int return value: %d\n", iRet);
			else if (nullptr != strstr(pszCmdline, "--test-float") && osgCmd_FloatValue("test_float", &fRet))
				printf("float return value: %.3f\n", fRet);
			else if (nullptr != strstr(pszCmdline, "--test-double") && osgCmd_DoubleValue("test_double", &dRet))
				printf("double return value: %.6f\n", dRet);
			else if (nullptr != strstr(pszCmdline, "--test-str"))
			{
				const char* szRet = osgCmd_StringValue("test_str");
				if (szRet && strcmp(szRet, "") != 0)
					printf("string return value: %s\n", szRet);
			}
			else if (nullptr != strstr(pszCmdline, "--test-err"))
			{
				const char* szErrMessage = osgCmd_ErrorMessage();
				if (szErrMessage && strcmp(szErrMessage, "") != 0)
					printf("error tip message: %s\n", szErrMessage);
			}
		}
	}
}

int main(int argc, char* args[])
{
	const char* datadir = nullptr;
	if (argc > 1)
		datadir = args[1];

	int cmdcount = argc - 2;
	char** cmdset = nullptr;
	if (cmdcount > 0)
	{
		cmdset = new char*[cmdcount];
		for (int i = 0; i < cmdcount; ++i)
			cmdset[i] = args[i + 2];
	}

	osgCmd_InitA(cmdcount, (const char**)cmdset, datadir);

	if (cmdset)
		delete[] cmdset;

	std::thread cmdline(parsecmdline);
	osgCmd_Run();
	cmdline.join();
	osgCmd_Destroy();

	return 0;
}
