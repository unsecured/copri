.PHONY: all clean install doc package copri test valgrind
default:    copri
all:	doc test install
copri:
	scons
test:
	scons --run-test
valgrind:
	scons --run-test --valgrind
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
doc:
	docco -L res/docco-lang.json -l linear README.md app.c array.c copri.c gen.c test/test-*.c
	cp docs/README.html docs/index.html
	cp res/runtime.png docs/runtime.png
	cat res/doc.css >> docs/docco.css