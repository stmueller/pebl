cat main.html | sed -e 's/\+Name\/Symbol//g' -e 's/<div class=\"p\"><!----><\/div>/<hr align=left  width=50\%>/g' > manual.html
