////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File: forallel.cpp
//  Author: David Steinberg
//  Date: May 8, 2012
//  Description: The functionality of the forallel tool.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//										INCLUDES
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <libxml/parser.h>
#include <string>
#include "forallel.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
//										ARGUMENT VALUES
////////////////////////////////////////////////////////////////////////////////////////////////////

bool			DETAILS = false;
const char *	fileName;

////////////////////////////////////////////////////////////////////////////////////////////////////
//										XML TAGS TO CHECK
////////////////////////////////////////////////////////////////////////////////////////////////////

xmlChar *	forTag		= xmlCharStrdup("for");
xmlChar *	initTag		= xmlCharStrdup("init");
xmlChar *	typeTag		= xmlCharStrdup("type");
xmlChar *	condTag		= xmlCharStrdup("condition");
xmlChar *	incrTag		= xmlCharStrdup("incr");
xmlChar *	exprStmtTag	= xmlCharStrdup("expr_stmt");
xmlChar *	exprTag		= xmlCharStrdup("expr");
xmlChar *	nameTag		= xmlCharStrdup("name");
xmlChar *	indexTag	= xmlCharStrdup("index");

////////////////////////////////////////////////////////////////////////////////////////////////////
//										BOOLEANS FOR STATE
////////////////////////////////////////////////////////////////////////////////////////////////////

bool	withinInit			= false,
		withinType			= false,
		withinCond			= false,
		withinIncr			= false,
		withinBlock			= false,
		withinExprStmt		= false,
		withinExpr			= false,
		withinName			= false,
		withinIndex			= false;		

////////////////////////////////////////////////////////////////////////////////////////////////////
//										LOOP INNARDS
////////////////////////////////////////////////////////////////////////////////////////////////////
	
ForLoop			*fLoop			= new ForLoop();
ArrayRef		*aRef			= NULL;

int				loopCount		= 0,
				depth			= 0,
				stmtNum			= 0;

std::string		arrayName;
	
bool			stmtHadArray	= false;
	
////////////////////////////////////////////////////////////////////////////////////////////////////
//										PROTOTYPES
////////////////////////////////////////////////////////////////////////////////////////////////////

void	argCheck		( int argc, char **argv );

void	startElement	( void *data, const xmlChar *fullname, const xmlChar **ats );
void	endElement		( void *data, const xmlChar *fullname );
void	characters		( void *data, const xmlChar *ch, int len );

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										MAIN
//
////////////////////////////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv ) {

// CHECK ARGUMENTS

	argCheck( argc, argv );

// SET SAX FUNCTIONS

	static xmlSAXHandler sax;
	
	sax.initialized = XML_SAX2_MAGIC;

	sax.startElement = startElement;
	sax.endElement = endElement;
	sax.characters = characters;

	char data; // [200]

// PARSE FILE

	int exitCode = xmlSAXUserParseFile ( &sax, &data, fileName );

	if ( exitCode != 0 ) {
		std::cerr << "\n\nError " << exitCode << "\n\n";
		exit(1);
	}
	
// END

	return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										ARG CHECK
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void argCheck ( int argc, char **argv ) {

// CHECK FOR DETAIL ARGUMENT

	if ( argc == 3 ) {
	
		if ( argv[1][0] == '-' ) {

			if ( argv[1][1] == 'd' )	DETAILS = true;
			fileName = argv[2];

		} else if ( argv[2][0] == '-' ) {
		
			if ( argv[2][1] == 'd' )	DETAILS = true;
			fileName = argv[1];			
		
		}

// ONLY A FILENAME IS PROVIDED
	
	} else if ( argc == 2 ) {
	
		fileName = argv[1];

// INVALID NUMBER OF ARGUMENTS
	
	} else {
	
		std::cerr	<< "\nUsage:\t" << argv[0] << " <filename>\n"
					<< "  or :\t" << argv[0] << " -d <filename>\n"
					<< "  or :\t" << argv[0] << " <filename> -d\n\n";
		exit(1);

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										START ELEMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void startElement ( void *data, const xmlChar *fullname, const xmlChar **ats ) {

// CHECK FOR TAGS
	
	if ( xmlStrEqual(fullname,forTag) == 1) { // enter the for loop

		withinBlock = false;

		if ( depth == 0 ) {
			loopCount++;
			stmtNum = 0;
		}

		fLoop->setDepth(depth);
		depth++;

	}

	else if ( xmlStrEqual(fullname,initTag) == 1 )		withinInit = true;
	else if ( xmlStrEqual(fullname,typeTag) == 1 )		withinType = true;
	else if ( xmlStrEqual(fullname,nameTag) == 1 )		withinName = true;
	else if ( xmlStrEqual(fullname,exprTag) == 1 )		withinExpr = true;
	else if ( xmlStrEqual(fullname,exprStmtTag) == 1 )	{ withinExprStmt = true; if(withinBlock) { stmtNum++; stmtHadArray = false; } }
	else if ( xmlStrEqual(fullname,condTag) == 1 )		{ withinCond = true; if(withinBlock) stmtNum++; }
	else if ( xmlStrEqual(fullname,incrTag) == 1 )		withinIncr = true;
	else if ( xmlStrEqual(fullname,indexTag) == 1 )		withinIndex = true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										END ELEMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void endElement ( void *data, const xmlChar *fullname ) {

// CHECK FOR TAGS

	if ( xmlStrEqual(fullname,forTag) == 1) {
		depth--;
		
		if ( DETAILS ) {
			std::cout << "----------------------------\n\nLoop " << loopCount << "\n\n";
			fLoop->printArrayRefs();
		}

		if ( depth == 0 ) {
			if ( fLoop->GCDTest() ) {
				if ( DETAILS ) {
					std::cout << "CANNOT be in parallel.\n\n";
				}
				exit(1);
			}
			else {
				if ( DETAILS ) {
					std::cout << "CAN be in parallel.\n\n";
				}
			}
		}

		delete fLoop;
		fLoop = new ForLoop;

	}

	else if ( xmlStrEqual(fullname,initTag) == 1 )		withinInit = false;
	else if ( xmlStrEqual(fullname,typeTag) == 1 )		withinType = false;
	else if ( xmlStrEqual(fullname,nameTag) == 1 )		withinName = false; // HAVE TO HAVE MORE BOOLEANS FOR THIS
	else if ( xmlStrEqual(fullname,exprTag) == 1 )		withinExpr = false;
	else if ( xmlStrEqual(fullname,exprStmtTag) == 1 )	withinExprStmt = false;
	else if ( xmlStrEqual(fullname,condTag) == 1 )		withinCond = false;
	else if ( xmlStrEqual(fullname,incrTag) == 1 )		{ withinIncr = false; withinBlock = true; }
	else if ( xmlStrEqual(fullname,indexTag) == 1 )		withinIndex = false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										CHARACTERS
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void characters ( void *data, const xmlChar *ch, int len ) {


// STRIP OUT WHITESPACE

	char chars[len];

	int newLen = 0;
	for ( int i = 0; i < len; i++) {
		chars[newLen] = ch[i];
		if ( !isspace(chars[newLen]) ) {
			newLen++;
		}
	}
	chars[newLen] = 0;

	std::string str(chars);

// CHECK WITHIN LOOP BLOCK
	
	if ( withinBlock) {

		if ( withinExpr || withinExprStmt || withinCond ) {

			if ( withinName ) {

				if ( withinIndex )	aRef->addIndexElem(str);
				else				arrayName = str;

			} else if ( withinIndex ) {

				if ( str.find('[') != std::string::npos ) {			// begin index
					delete aRef;
					aRef = new ArrayRef(arrayName);
					aRef->setStmtNum(stmtNum);
					
					stmtHadArray = true;
				}

				else if ( str.find(']') != std::string::npos ) {	// end index

					aRef->parseIndex();
					fLoop->addArrRead(*aRef);

				}

				else {												// inside index

					size_t start = 0;
					for ( size_t i = 0; i < str.length(); i++ ) {
					
						if ( str[i] == '*' || str[i] == '+' || str[i] == '-' ) { // math operators
						
							if ( i != 0 ) {							
								aRef->addIndexElem(str.substr(start,(i-start)));
							}

							aRef->addIndexElem(str.substr(i,1));
							start = i+1;
						
						} else if ( i == str.length() - 1 ) {						
							aRef->addIndexElem(str.substr(start));
						}
					
					}
					
				}

			} else {
				if (
					str.find('=') != std::string::npos &&		// it includes an equals sign
					str.find('"') == std::string::npos &&		// and is not a printout
					str.find("==") == std::string::npos &&		// or a boolean check
					str.find("!=") == std::string::npos &&		// or a boolean check
					stmtHadArray								// and there is an assumed read to fix
				) {
		
					fLoop->addArrWrite(fLoop->popLastRead());
					
				}

			}

		} // END WITHIN EXPR
	
	} // END WITHIN BLOCK

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										END FORALLEL.CPP
////////////////////////////////////////////////////////////////////////////////////////////////////