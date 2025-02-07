HOST = "has_delete.com"

TYPE = "POST"
URI = "/here_is_delete_allowed/delete_me.txt"
BODY = "posted by post, fine to delete"
CONTENT_LENGTH = str(len(BODY))

http_request = """\
{type} {uri} HTTP/1.1\r
Host: {host}\r
User-Agent: CustomClient/1.0\r
Connection: close\r
Content-Length: {content_length}\r
\r
{body}""".format(
    type=TYPE,
    uri=URI,
    host=HOST,
    body=BODY,
    content_length=CONTENT_LENGTH
)
