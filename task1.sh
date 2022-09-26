# remove invalid characters
# uniq
cat ./dirty/* > dirty.txt
cp dirty.txt  clean.txt
sed -i s/[^[:alpha:]]//g clean.txt
# remove words < 3 length
sed -i -e '/^.{,3}$/d' clean.txt
sed -i -r '/^.{15,}./d' clean.txt
shuf clean.txt --output=clean.txt