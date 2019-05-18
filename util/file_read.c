#include "../include/file_read.h"

/**
 * file Read 함수
 *
 * @param   token       파일 read 결과를 저장할 구조체의 주소i
 * @param   filePath    파일 위치 스트링
 * @return  int         파일 read 성공 여부 반환 ( SUCCESS : 0, FAIL : -1 )
*/
int fileRead(token *token, const char *filePath)
{
	FILE *filePointer = NULL;
    struct stat fileBuffer;
    int fileReadResult = 0;
    unsigned long freadResult = 0; // fread 함수 결과
    unsigned long fileByteSize = 0; // stat에서 파일 길이 반환 결과

	/* param exception */
    if (token == NULL) {
        printf("[Exception] param is null pointer. (fileRead function, token)\n");
        return FAIL;
    }
    if (filePath == NULL) {
        printf("[Exception] param is null pointer. (fileRead function, filePath)\n");
        return FAIL;
    }

#ifdef DEBUG_FILE_READ_C
    printf("### [DEBUG] --- fileRead start ---\n");
#endif

    fileReadResult = SUCCESS;
    initToken(token);

    /* get file size */
    if (stat(filePath, &fileBuffer) == GET_FILE_STAT_FAIL) {
    	printf("[Exception] get file status fail. (%s) \n", filePath);
		fileReadResult = FAIL;
        goto fileReadFail;
	}
    fileByteSize = fileBuffer.st_size;
	
    /* file pointer set */
	filePointer = fopen(filePath, "rb"); // b : '\r', '\n' 같은 문자를 구별해서 저장하기 위함
	if (filePointer == NULL) {
		printf("[Exception] file pointer is null pointer. (%s) \n", filePath);
		fileReadResult = FAIL;
        goto fileReadFail;
	}

	/* token->value allocation */
    token->value = (char*)calloc(1, sizeof(char) * fileByteSize);
	if (token->value == NULL) {
		printf("[Exception] memory allocation fail.. \n");
		fileReadResult = FAIL;
        goto fileReadFail;
	}

	/* file read and save ( 성공시 파일 길이 반환 ) */
    freadResult = fread(token->value, sizeof(char), fileByteSize, filePointer);
	if (fileByteSize != freadResult) {
		printf("[Exception] file data get fail.. (current : %ld, required : %ld)\n", freadResult, fileByteSize);
        fileReadResult = FAIL;
		goto fileReadFail;
	}

    token->size = fileByteSize;

#ifdef DEBUG_FILE_READ_C
    viewToken(token);
    printf("\n### [DEBUG] --- fileRead end ---\n");
#endif

fileReadFail:

    /* clean up */
	if (fileReadResult == FAIL) { // 만약 fread 실패시 toeken->value memeory free
        deleteTokenValue(token);
    }
    if (filePointer != NULL) {
		fclose(filePointer);
	}
	return fileReadResult;
}

void clearInputBuffer(void)
{
    char bufferTemp = 0;
    while (1) {
        bufferTemp = getchar();
        if (bufferTemp == '\n') {
            break;
        }
    }
    return;
}

