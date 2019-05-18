# 프로젝트 제목 
 
 * HTTP Request Parsing Program + Server Round_Robin Scheduling Program 

# 프로젝트 설명

<pre>

 -- RFC -- 

request         =   request-line CRLF
                    requset-headr CRLF
                    CRLF
                    garbage proceesing..

request-line    =   Method SP URI SP  HTTP-VERSION CRLF

URI             =   PATH(absolute) ["?" QUERY] [ "#" FRAMGENT ]

PATH(absolute)  =   "/" [ 1*pchar *( "/" *pchar ) ]

-- confing --

cofing          =   *(TYPE SP MATCH SP MATCH_STRING SP 1*(server_ip) CRLF)

TYPE            =   "HOST" | "PATH"

MATCH           =   "any" | "start" | "end"

MATH_STRING     =   1*pchar


주어진 RFC 규격의 request packet 을 각 형태에 맞게 파싱
파싱 후 confing filq 에 있는 HOST 또는 URI-PATH에 매칭 되는 각각의 서버를 라운드 로빈 스케쥴링 

</pre>

