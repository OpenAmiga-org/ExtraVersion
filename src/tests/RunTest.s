Echo "About to run tests..."

Echo "Testing good:"

; Pass 0 of goods

Echo Noline 0.1-
Set ans `ExtVersion tests/good/testfile.good.0.1`
If "$ans" Matches "Extversion 0.3-Alpha_3.1"
	Echo " OK"
Else
	Skip failed
Endif

; Pass 1 of goods

Echo Noline 1.1-
Set ans `ExtVersion tests/good/testfile.good.1.1`
If "$ans" Matches "Extversion 0.3-Alpha_3"
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.2-
Set ans `ExtVersion FULL tests/good/testfile.good.1.1`
If "$ans" Eq "Extversion 0.3-Alpha_3 (12.05.2014) Extversion test comment."
; >>NOTE: The comment text comes on the same line without a newline
; >>unlike in the command Version.<<
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.3-
Set ans `ExtVersion VSTRING tests/good/testfile.good.1.1`
If "$ans" Eq "$EXTVER: Extversion 0.3-Alpha_3 (12.05.2014) Extversion test comment."
	; >>NOTE: The space is included between '.2014)' and 'Version' <<
	; >>unlike in the command Version.<<
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.4-
Set ans `ExtVersion tests/good/testfile.good.1.1 full`
If "$ans" Eq "Extversion 0.3-Alpha_3 (12.05.2014) Extversion test comment."
	; >>NOTE: The comment text comes on the same line without a newline
	; >>unlike in the command Version.<<
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.5-
Set ans `ExtVersion tests/good/testfile.good.1.1 vstring`
If "$ans" Eq "$EXTVER: Extversion 0.3-Alpha_3 (12.05.2014) Extversion test comment."
	; >>NOTE: The space is included between '.2014)' and 'Version' <<
	; >>unlike in the command Version.<<
	Echo " OK"
Else
	Skip failed
Endif

;full: Extversion 0.3-Alpha_3 (12.05.2014) Extversion test comment.

; Pass 2 of goods



; -- --

Echo "Testing bad:"

; Pass 1 of bads

;Echo Noline 0-
;Set ans `ExtVersion tests/bad/testfile.bad.1.1`
;If "$ans" Eq ""
;	Echo " OK"
;Else
;	Skip failed
;Endif

;Echo Noline 1.1-
;Set ans `ExtVersion`
;If "$ans" Eq "EXTVERSION: object not found"
;	Echo " OK"
;Else
;	Skip failed
;Endif
Echo Noline 1.2-
Set ans `ExtVersion file.NoSuchFile`
If "$ans" Eq "EXTVERSION: object not found"
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.3-
Set ans `ExtVersion FULL tests/bad/testfile.bad.NoSuchFile`
If "$ans" Eq "EXTVERSION: object not found"
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.4-
Set ans `ExtVersion NO_COMMAND tests/bad/testfile.bad.NoSuchFile`
If "$ans" Eq "EXTVERSION: object not found"
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.5-
Set ans `ExtVersion NO_COMMAND tests/bad/testfile.bad.NoSuchFile full`
If "$ans" Eq "EXTVERSION: object not found"
	Echo " OK"
Else
	Skip failed
Endif
Echo Noline 1.6-
Set ans `ExtVersion tests/bad/testfile.bad.NoSuchFile NO_COMMAND NO_COMMAND`
If "$ans" Eq "EXTVERSION: object not found"
	Echo " OK"
Else
	Skip failed
Endif

; Pass 2 of bads

Echo Noline 2.1-
Set ans `ExtVersion tests/bad/testfile.bad.2.1`
If "$ans" Eq ""
	Echo " OK"
Else
	Skip failed
Endif

Skip exit

Lab failed
Echo " FAILED"
Echo Noline "Output was (without ' chars): "
Echo "'$ans'"
Echo "Aborting now with a Quit 10..."
Quit 10

Lab exit
Echo "All tests succeeded."