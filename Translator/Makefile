#***********************************************************
# Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
# 4-as praktinis darbas.
# Darbà atliko: Jonas Inèius
#***********************************************************

uzd4: lex.yy.c yacc.tab.c main.o Attribute.o Selector.o Node.o CodeGen.o vector.o CppClass.o VariableDeclaration.o MethodDeclaration.o MethodImplementation.o ArithmeticTranslator.o
	gcc -o uzd4 lex.yy.c yacc.tab.c main.o Attribute.o Selector.o Node.o CodeGen.o vector.o CppClass.o VariableDeclaration.o MethodDeclaration.o MethodImplementation.o ArithmeticTranslator.o

main.o: main.c CodeGen.h 
	gcc -o main.o -c main.c
	
lex.yy.c: lex.lex
	flex -o lex.yy.c lex.lex
	
yacc.tab.c: yacc.y ArithmeticTranslator.h Attribute.h CodeGen.h Node.h Selector.h vector.h
	bison -o yacc.tab.c -d -v -t yacc.y 
	
Attribute.o: Attribute.c Attribute.h vector.h
	gcc -o Attribute.o -c Attribute.c

Selector.o: Selector.c Selector.h Attribute.h
	gcc -o Selector.o -c Selector.c
	
Node.o: Node.c Node.h Attribute.h Selector.h vector.h
	gcc -o Node.o -c Node.c
	
CodeGen.o: CodeGen.c CodeGen.h CppClass.h Node.h Selector.h vector.h
	gcc -o CodeGen.o -c CodeGen.c
	
vector.o: vector.c vector.h
	gcc -o vector.o -c vector.c

CppClass.o: CppClass.c CppClass.h MethodImplementation.h MethodDeclaration.h VariableDeclaration.h vector.h
	gcc -o CppClass.o -c CppClass.c

VariableDeclaration.o: VariableDeclaration.c VariableDeclaration.h
	gcc -o VariableDeclaration.o -c VariableDeclaration.c

MethodDeclaration.o: MethodDeclaration.c MethodDeclaration.h MethodImplementation.h VariableDeclaration.h vector.h
	gcc -o MethodDeclaration.o -c MethodDeclaration.c
	
MethodImplementation.o: MethodImplementation.c MethodImplementation.h VariableDeclaration.h MethodDeclaration.h vector.h
	gcc -o MethodImplementation.o -c MethodImplementation.c

ArithmeticTranslator.o: ArithmeticTranslator.c ArithmeticTranslator.h vector.h
	gcc -o ArithmeticTranslator.o -c ArithmeticTranslator.c
	
	
clean:
	rm *.o
	
