.PHONY: all clean install doc package copri
default:    copri
all:	doc test install
copri:
	scons
test:
	scons --test
clean:
	scons -c --test
	rm -rf docs
	rm -f copri.tar.gz
install: 
	cp app /usr/local/bin/copri
package:	clean
	mkdir -p .package
	cp -r * .package
	rm -f .package/deploy* .package/*.lst .package/*.o .package/*.a .package/._*
	mv .package copri
	tar cvzf copri.tar.gz copri
	rm -rf copri
doc:	package
	docco -L res/docco-lang.json -l linear README.md app.c array.c copri.c gen.c test-*.c
	cp docs/README.html docs/index.html
	cp res/runtime.png docs/runtime.png
	cp res/p1024_x*.lst docs
	gzip docs/p1024_x*.lst
	cat res/doc.css >> docs/docco.css
	mv copri.tar.gz docs/copri.tar.gz