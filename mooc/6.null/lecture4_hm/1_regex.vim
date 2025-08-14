Lesson:https://regexone.com/lesson/introduction_abcs
"lesson1:an introdution , and the ABCs"

Exercise 1: Matching Characters
Task	Text	 
Match	abcdefg	Success
Match	abcde	Success
Match	abc     Success

solution:abc

"don't use something higher ?"
"i see ,find the common parts"

"lesson  1½:The 123s"
"metacharacter \d can be used in place of any digit "
"metacharacter \D can be used in place of any non-digit characters"

Exercise 1½: Matching Digits
Task	Text	 
Match	abc123xyz	Success
Match	define "123"	Success
Match	var g = 123;	Success

solution: 123

"lesson2: The Dot"
"character .means Any Character
"character \. means Period(句点)"

Exercise 2: Matching With Wildcards
Task	Text	 
Match	cat.	Success
Match	896.	Success
Match	?=+.	Success
Skip	abc1	Skip

solution:...\.

"Lesson 3: Matching specific characters"
"[abc]	means Only a single a, b, or c"

Exercise 3: Matching Characters
Task	Text	 
Match	can	Success
Match	man	Success
Match	fan	Success
Skip	dan Skip	
Skip	ran Skip	
Skip	pan Skip	

solution: [cmf]an

"Lesson 4: Excluding specific characters"
"[^abc] means not a,b,or c"

Exercise 4: Excluding Characters
Task	Text	 
Match	hog	Success
Match	dog	Success
Skip	bog	Skip

solution:[^b]og

"Lesson 5: Character ranges"
"[a-z]	Characters a to z"
"[0-9]	Numbers 0 to 9"
"[^a-c] only match any single character except letters a-c"
"\w	Any Alphanumeric character is equivalent to the character range [A-Za-z0-9]"
"\W	Any Non-alphanumeric character"
"alphanumeric:字母数字的"

Exercise 5: Matching Character Ranges
Task	Text	 
Match	Ana	Success
Match	Bob	Success
Match	Cpc	Success
Skip	aax Skip	
Skip	bby Skip	
Skip	ccz Skip

solution:[A-C][n-p][a-c]

"Lesson 6: Catching some zzz's"
"character{m}	character will have m Repetitions"
"character{m,n}	character will have m to n Repetitions (m<=x<=n)"

Exercise 6: Matching Repeated Characters
Task	Text	 
Match	wazzzzzup	Success
Match	wazzzup		Success
Skip	wazup		Skip	

solution: waz{2,5}up

"Lesson 7: Mr. Kleene, Mr. Kleene"
"*	Zero or more repetitions"
"+	One or more repetitions"

Exercise 7: Matching Repeated Characters
Task	Text	 
Match	aaaabcc	Success
Match	aabbbbc	Success
Match	aacc	Success
Skip	a		Skip	

solution:aa+b*c+

"Lesson 8: Characters optional"
"?	Optional character"
"For example, the pattern ab?c will match either the strings "abc" or "ac" because the b is considered optional"
"\? is literal ?"

Exercise 8: Matching Optional Characters
Task	Text	 
Match	1 file found?	Success
Match	2 files found?	Success
Match	24 files found?	Success
Skip	No files found	

solution:\d+ files? found\?

"Lesson 9: All this whitespace"
"\s	Any Whitespace"
"\S	Any Non-whitespace character"

Exercise 9: Matching Whitespaces
Task	Text	 
Match	1. abc		    	Success
Match	2.	abc				Success
Match	3.           abc	Success
Skip	4.abc	

solution:\d\.\s+abc

"Lesson 10: Starting and ending"
" ^:starts $:ends"

Exercise 10: Matching Lines
Task	Text	 
Match	Mission: successful								Success
Skip	Last Mission: unsuccessful	
Skip	Next Mission: successful upon capture of target

solution:^Mission: successful$

"Lesson 11: Match groups"
"():capture groups"

Exercise 11: Matching Groups
Task	Text	Capture Groups	 
Capture	file_record_transcript.pdf	file_record_transcript	Success
Capture	file_07241999.pdf	file_07241999					Success
Skip	testfile_fake.pdf.tmp								Skip	

solution:^(file.+)\.pdf$

"Lesson 12: Nested groups"
"(a(bc))	Capture Sub-group"
Exercise 12: Matching Nested Groups
Task	Text	Capture Groups	 
Capture	Jan 1987	Jan 1987 1987	Success
Capture	May 1969	May 1969 1969	Success
Capture	Aug 2011	Aug 2011 2011	Success

solution: ^(\w+ (\d+))$

"Lesson 13: More group work"
"(.*)	Capture all"

Exercise 13: Matching Nested Groups
Task	Text	Capture Groups	 
Capture	1280x720	1280 720	Success
Capture	1920x1600	1920 1600	Success
Capture	1024x768	1024 768	Success

solution:^(\d+)x(\d+)$

"Lesson 14: It's all conditional"

Exercise 14: Matching Conditional Text
Task	Text	 
Match	I love cats	Success
Match	I love dogs	Success
Skip	I love logs Skip	
Skip	I love cogs Skip	

solution:I love (ca|do)(g|t)s

"Lesson 15: Other special characters"
"\b which matches the boundary between a word and a non-word character. for example by using the pattern \w+\b"
"back slash '\' + num reference your captured groups by using \0 (usually the full matched text), \1 (group 1), \2 (group 2)"

Exercise 15: Matching Other Special Characters
Task	Text	 
Match	The quick brown fox jumps over the lazy dog.					Success
Match	There were 614 instances of students getting 90.0% or above.	Success
Match	The FCC had to censor the network for saying &$#*@!.			Success

mysolution:The(\w|\s|.)+

"Lesson X: Infinity and beyond!"
4$.. haha


Problem:https://regexone.com/problem/matching_decimal_numbers

"Problem 1: Matching a decimal numbers"
" Notice how you will have to match the decimal point itself and not an arbitrary character using the dot metacharacte"

Exercise 1: Matching Numbers
Task	Text	 
Match	3.14529		Success
Match	-255.34		Success
Match	128			Success
Match	1.9e10		Success
Match	123,340.00	Success

mysolution:^-?\d+(,\d+)*?\.?\d+e?\d+$
solution: ^-?\d+(,\d+)*(\.\d+(e\d+)?)?$ 


"Problem 2: Matching phone numbers"
"Below are a few phone numbers that you might encounter when using real data, write a single regular expressions that matches the number and captures the proper area code."


Exercise 2: Matching Phone Numbers
Task	Text			Capture Groups	 
Capture	415-555-1234	415					Success
Capture	650-555-2345	650					Success
Capture	(416)555-3456	416					Success
Capture	202 555 4567	202					Success
Capture	4035555678		403					Success
Capture	1 416 555 9292	416					Success


mysolution:(\d\s)?\(?(\d{3})\)?((-|\s)?\d+)*
solution: 1?[\s-]?\(?(\d{3})\)?[\s-]?\d{3}[\s-]?\d{4}.

"Problem 3: Matching emails"
"Below are a few common emails, in this example, try to capture the name of the email, excluding the filter (+ character and afterwards) and domain (@ character and afterwards)."


Exercise 3: Matching Emails
Task	Text								Capture Groups	 
Capture	tom@hogwarts.com					tom						Success
Capture	tom.riddle@hogwarts.com				tom.riddle				Success
Capture	tom.riddle+regexone@hogwarts.com	tom.riddle				Success
Capture	tom@hogwarts.eu.com					tom						Success
Capture	potter@hogwarts.com					potter					Success
Capture	harry@hogwarts.com					harry					Success
Capture	hermione+regexone@hogwarts.com		hermione				Success

mysolution:(\w+\.?\w+)(\+\w+)?@\w+(\.\w+)*\.com

"Problem 4: Matching HTML"
"Go ahead and write regular expressions for the following examples."


Exercise 4: Capturing HTML Tags
Task	Text									Capture Groups	 
Capture	<a>This is a link</a>					a					Success
Capture	<a href='https://regexone.com'>Link</a>	a					Success
Capture	<div class='test_style'>Test</div>		div					Success
Capture	<div>Hello <span>world</span></div>		div					Success

mysolution: </(\w{1,3})>$
solution:<(\w+).


"Problem 5: Matching specific filenames"
"In this simple example, extract the filenames and extension types of only image files (not including temporary files for images currently being edited). Image files are defined as .jpg,.png, and .gif."

Exercise 5: Capturing Filename Data
Task	Text					Capture Groups	 
Skip	.bash_profile		
Skip	workspace.doc		
Capture	img0912.jpg						img0912 jpg		Success
Capture	updated_img0912.png		updated_img0912 png		Success
Skip	documentation.html		
Capture	favicon.gif						favicon gif		Success
Skip	img0912.jpg.tmp		
Skip	access.lock	

mysolution:^([iuf]\w+)\.(.{3})$
solution: (\w+)\.(jpg|png|gif)$

"Problem 6: Trimming whitespace from start and end of line"
"Write a simple regular expression to capture the content of each line, without the extra whitespace."

Exercise 6: Matching Lines
Task	Text								Capture Groups	 
Capture				The quick brown fox...	The quick brown fox...		Success
Capture	   jumps over the lazy dog.			jumps over the lazy dog.	Success

"Problem 7: Extracting information from a log file"
"Your goal is to use any regular expression techniques that we've learned so far to extract the filename, method name and line number of line of the stack trace (they follow the form "at package.class.methodname(filename:linenumber)")."

Exercise 7: Extracting Data From Log Entries
Task	Text															Capture Groups	 
Skip	W/dalvikvm( 1553): threadid=1: uncaught exception		
Skip	E/( 1553): FATAL EXCEPTION: main		
Skip	E/( 1553): java.lang.StringIndexOutOfBoundsException		
Capture	E/( 1553):   at widget.List.makeView(ListView.java:1727)		makeView ListView.java 1727	Success
Capture	E/( 1553):   at widget.List.fillDown(ListView.java:652)			fillDown ListView.java 652	Success
Capture	E/( 1553):   at widget.List.fillFrom(ListView.java:709)			fillFrom ListView.java 709  Success	

mysolution:\.(\w+)\((\S+):(\d+)\)$
solution: (\w+)\(([\w\.]+):(\d+)\)


"Problem 8: Parsing and extracting data from a URL"

"URIs, or Uniform Resource Identifiers, are a representation of a resource that is generally composed of a scheme, host, port (optional), and resource path, respectively highlighted below.

"http://regexone.com:80/page
"The scheme describes the protocol to communicate with, the host and port describe the source of the resource, and the full path describes the location at the source for the resource.

"In the exercise below, try to extract the protocol, host and port of the all the resources listed.


Exercise 8: Extracting Data From URLs
Task	Text																	Capture Groups	 
Capture	ftp://file_server.com:21/top_secret/life_changing_plans.pdf				ftp file_server.com 21			Success
Capture	https://regexone.com/lesson/introduction#section						https regexone.com				Success
Capture	file://localhost:4040/zip_file											file localhost 4040				Success
Capture	https://s3cur3-server.com:9999/											https s3cur3-server.com 9999	Success
Capture	market://search/angry%20birds											market search					Success

mysolution:^(\w+)://(\w+-?\w*(.com)?):?(\d+)?
solution:(\w+)://([\w\-\.]+)(:(\d+))?


"Problem X: Infinity and beyond!"
4$... hahaha




