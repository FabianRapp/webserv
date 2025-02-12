# webserv


1. CGI variables.
2. HTML file to select file to upload.
3. CGI script to return 505 file or some kinda of simple .html
4. GCI GET and POST. Scripts to to that.

And config file for those CGI locations.


------------------

How to use CURL:
https://acquia.my.site.com/s/article/360005257154-Use-cURL-s-resolve-option-to-pin-a-request-to-an-IP-address

./webserv config/cgitest.conf
curl http://www.kotlet.com:9090 --resolve www.kotlet.com:9090:127.0.0.1

curl http://magicform.com:8080 --resolve magicform.com:8080:127.0.0.1

curl http://www.getbetter.com:8080 --resolve www.getbetter.com:8080:127.0.0.1

curl http://magicform.com:8080 --resolve magicform.com:8080:127.0.0.1

curl http://magicform.com:9090 --resolve magicform.com:9090:127.0.0.1

curl http://www.^-getbetter.com:9090 --resolve www.^-getbetter.com:9090:127.0.0.1

curl http://www.getbetter.com:9090 --resolve www.getbetter.com:9090:127.0.0.1

telnet?


----------------
CGI:
You need to test with files containing errors to see if the error handling works properly. You can use a script containing an infinite loop or an error; you are free to do whatever tests you want within the limits of acceptability that remain at your discretion. The group being evaluated should help you with this.



------------------

server_name validation.
request_body_size should be in location also.
add PUT method to allowed_methods for both.


bool CGIManager::isCGI(const std::string& path, const LocationConfigFile& location_config) {
	finish validating .py | .php | .bla

TODO:
1. CHUNKS.
2. ERROR handleling.
3. FRONT-END for the evaluation to have every test avaialbe.
4. Implement PUT.


TODO:
1. PUT file name (do we care if we have different test);
2. ERROR codes gives wierd files? Something is handleing like it should



TODO:
<!-- 1. change default error pages to have no images and no links. -->
	<!-- a. server_name validation recheck (has_name.com) does not work for some reason. -->
	<!-- b. condif error_pages does not work. We need to fix that. Parsing or logic in the DefaultErrorPages.hpp -->
2. chunked need to be tested more. need to figure it out how.

4. front-end presentation for eval.
5. Compile warning.
6. Prepare cool scripts using CGI (py, php, bash)
8. go throught evaluation sheet
9. check for leaks (valgrind)
10. try to brake the webserver somehow
12. try to break config file

13. Try chmod on server files and make sure nothing crashes
14. filesystem::filesystem_error in posix_fstat filename too long
