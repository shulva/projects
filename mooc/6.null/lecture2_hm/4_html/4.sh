#!/bin/bash 
	find . -name '*.html' | xargs -d '\n' zip -r html.zip 
	unzip -l ./html.zip 
