////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File: forallel.hpp
//  Author: David Steinberg
//  Date: May 8, 2012
//  Description: Data structures for forallel.cpp
//
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//										INCLUDE GUARDS
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FORALLEL_HPP_
#define _FORALLEL_HPP_

////////////////////////////////////////////////////////////////////////////////////////////////////
//										INCLUDES
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cmath>
#include <cctype>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										INDEX VARIABLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class IndexVar {
public:
					IndexVar	()												{ coef = 1; };
					IndexVar	( const int c, const std::string n )			{ coef = c; name = n; };

					IndexVar	( const IndexVar& actual )						{ coef = actual.coef; name = actual.name; };
					~IndexVar	()												{ coef = 0; };
	
	void			setCoef		( const int c )									{ coef = c; };
	int				getCoef		()										const	{ return coef; };

	void			setName		( const std::string n )							{ name = n; };
	std::string		getName		()										const	{ return name; };

private:
	int				coef;
	std::string		name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										ARRAY REFERENCE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class ArrayRef {
public:
								ArrayRef		() 									{ constant = 0; stmtNum = -1; };
								ArrayRef		( const std::string& n )			{ name = n; constant = 0; stmtNum = -1; };

								ArrayRef		( const ArrayRef& actual )			{ name = actual.name; indexElems = actual.indexElems; vars = actual.vars; constant = actual.constant; stmtNum = actual.stmtNum; };
								~ArrayRef		()									{ constant = 0; };

	void						setName			( const std::string n )				{ name = n; };
	std::string					getName			() 							const	{ return name; };

	void						addIndexElem	( const std::string& e )			{ indexElems.push_back(e); };
	std::vector<std::string>*	getIndexElems	()									{ return &indexElems; };

	void						parseIndex		();

	void						addVar			( const IndexVar& iVar )			{ vars.push_back(iVar); };
	std::vector<IndexVar>*		getVars			()									{ return &vars; };

	void						consPlus		( const int c )						{ constant += c; };
	int							getCons			()							const	{ return constant; };

	void						setStmtNum		( const int n )						{ stmtNum = n; };
	int							getStmtNum		()							const	{ return stmtNum; };

	void						print			();

private:
	std::string					name;
	std::vector<std::string>	indexElems;
	std::vector<IndexVar>		vars;
	int							constant,
								stmtNum;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//										PRINT
////////////////////////////////////////////////////////////////////////////////////////////////////

void ArrayRef::print () {

	std::cout << stmtNum << ": " << name << "[";

	int i = 0;
	std::vector<IndexVar>::iterator		var = vars.begin();

	while ( var < vars.end() ) {	

		if ( var->getCoef() != 1 ) {

			if ( var->getCoef() == -1 ) std::cout << "-";
			else {

				if ( var->getCoef() > 0 && i > 0 ) std::cout << "+";
				std::cout << var->getCoef() << "*";

			}

		} else {
		
			if ( i > 0 ) std::cout << "+";
		
		}

		std::cout << var->getName();
		
		var++;
		i++;

	}

	if ( constant > 0 && i > 0 ) std::cout << "+";
	if ( constant ) std::cout << constant;

	std::cout << "]";
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										TO INT
////////////////////////////////////////////////////////////////////////////////////////////////////

int toInt ( std::string str ) {

	int	len		= str.length(),
		i		= len - 1,
		num		= 0,
		result	= 0;

	while ( i >= 0 ) {

		if ( i == 0 && str[i] == '-' ) {
			result -= (result * 2);
			break;			
		}

		int power = pow( 10, (len-1-i) );

		num = ( int(str[i]) - int('0') ) * power;
		result += num;
		i--;

	}

	return result;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										PARSE INDEX
////////////////////////////////////////////////////////////////////////////////////////////////////

void ArrayRef::parseIndex() {

// MULTIPLICATION

	for ( size_t i = 0; i < indexElems.size(); i++ ) {
	
		if ( indexElems[i].compare("*") == 0 ) {
		
			IndexVar iVar;
		
			if ( isdigit(indexElems[i-1][0]) ) { // COEF TO LEFT
			
				if ( i > 1 && indexElems[i-2].compare("-") == 0 ) {
					indexElems[i-2] = "@";
					indexElems[i-1] = "-" + indexElems[i-1];
				}
			
				iVar.setCoef(toInt(indexElems[i-1]));
				iVar.setName(indexElems[i+1]);
			
			} else {
			
				iVar.setCoef(toInt(indexElems[i+1]));
				iVar.setName(indexElems[i-1]);
			
			}
		
			addVar(iVar);
			
			indexElems[i-1] = "@";
			indexElems[i+1] = "@";
		
		}
		
	}

// SINGLE VARIABLES
	
	for ( size_t i = 0; i < indexElems.size(); i++ ) {
	
		if ( isalpha(indexElems[i][0]) || indexElems[i][0] == '_' ) {
		
			IndexVar iVar;
			
			iVar.setName(indexElems[i]);
			
			if ( i > 0 && indexElems[i-1].compare("-") == 0 ) {
				indexElems[i-1] = "@";
				iVar.setCoef(-1);
			}
		
			addVar(iVar);
			
			indexElems[i] = "@";
		
		}
	
	}

// SUBTRACTION
	
	for ( size_t i = 0; i < indexElems.size(); i++ ) {
		
		if ( indexElems[i].compare("-") == 0 ) {
			
			if ( isdigit(indexElems[i+1][0]) ) { // CONS TO RIGHT
			
				consPlus(toInt(indexElems[i+1])*-1);
			
			}	
		
		}
		
	}

// ADDITION
	
	for ( size_t i = 0; i < indexElems.size(); i++ ) {
	
		if ( indexElems[i].compare("+") == 0 ) {
		
			if ( isdigit(indexElems[i-1][0]) ) { // CONS TO LEFT?
			
				consPlus(toInt(indexElems[i-1]));
			
			} else if ( isdigit(indexElems[i+1][0]) ) { // CONS TO RIGHT?
			
				consPlus(toInt(indexElems[i+1]));
			
			}
	
		
		}
	
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										LOOP VARIABLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class LoopVar {
public:
					LoopVar			()									{ lowerBound = NULL; upperBound = NULL; step = NULL; isLoopCond = false; };

					LoopVar			( const LoopVar& actual )			{ name = actual.name; lowerBound = actual.lowerBound; upperBound = actual.upperBound; step = actual.step; isLoopCond = actual.isLoopCond; };
					~LoopVar		()									{};

	void			setName			( const std::string n )				{ name = n; };
	std::string		getName			() 							const	{ return name; };

	void			setLowerBound	( const int lb )					{ lowerBound = lb; };
	int				getLowerBound	()							const	{ return lowerBound; };

	void			setUpperBound	( const int ub )					{ upperBound = ub; };
	int				getUpperBound	()							const	{ return upperBound; };

	void			setStep			( const int s )						{ step = s; };
	int				getStep			()							const	{ return step; };

	void			setAsCond	()										{ isLoopCond = true; };
	bool			isCond		()								const	{ return isLoopCond; };

private:
	std::string		name;
	int				lowerBound,
					upperBound,
					step;
	bool			isLoopCond;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//										FOR LOOP
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class ForLoop {
public:
								ForLoop			()							{ parent = NULL; depth = -1; };

								ForLoop			( const ForLoop& actual )	{ parent = actual.parent; loopVars = actual.loopVars; arrWrites = actual.arrWrites; arrReads = actual.arrReads; depth = actual.depth; };
								~ForLoop		()							{ depth = -1; };

	void						setParent		( ForLoop& p )				{ parent = &p; };
	ForLoop*					getParent		()							{ return parent; };

	void						addLoopVar		( const LoopVar& lVar )		{ loopVars.push_back(lVar); };
	std::vector<LoopVar>*		getLoopVars		()							{ return &loopVars; };

	void						addArrWrite		( const ArrayRef& aRef )	{ arrWrites.push_back(aRef); };
	std::vector<ArrayRef>*		getArrWrites	()							{ return &arrWrites; };
	ArrayRef					popLastWrite	()							{ ArrayRef aRef(arrWrites.back()); arrWrites.pop_back(); return aRef; };

	void						addArrRead		( const ArrayRef& aRef )	{ arrReads.push_back(aRef); };
	std::vector<ArrayRef>*		getArrReads		()							{ return &arrReads; };
	ArrayRef					popLastRead		()							{ ArrayRef aRef(arrReads.back()); arrReads.pop_back(); return aRef; };

	void						setDepth		( const int d )				{ depth = d; };
	int							getDepth		()							{ return depth; };

	void						printArrWrites	();
	void						printArrReads	();
	void						printArrayRefs	();

	bool						GCDTest			();

private:
	ForLoop*					parent;
	std::vector<LoopVar>		loopVars;
	std::vector<ArrayRef>		arrWrites,
								arrReads;
	int							depth;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//										GCD HELPER
////////////////////////////////////////////////////////////////////////////////////////////////////

int GCD ( int x, int y ) {

	if ( x == y )	return x;

	int tmp;
	while ( 1 ) {
	
		if ( y == 0 )	return x;
		
		tmp = x % y;
		x = y;
		y = tmp;
	
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										GCD TEST
////////////////////////////////////////////////////////////////////////////////////////////////////

bool ForLoop::GCDTest () {

	std::vector<ArrayRef>::iterator		writeRef = arrWrites.begin(),
										writeRef2,
										readRef;

	while ( writeRef < arrWrites.end() ) {	

		writeRef2 = writeRef+1;
		while ( writeRef2 < arrWrites.end() ) {
		
			if ( writeRef->getName() == writeRef2->getName() ) {

				std::vector<int> coefs;
				
				std::vector<IndexVar>::iterator		writeVar = writeRef->getVars()->begin(),
													writeVar2 = writeRef2->getVars()->begin();

				while ( writeVar < writeRef->getVars()->end() ) {
					coefs.push_back(writeVar->getCoef());
					writeVar++;
				}
				while ( writeVar2 < writeRef2->getVars()->end() ) {
					coefs.push_back(writeVar2->getCoef());
					writeVar2++;
				}

				std::vector<int>::iterator coef = coefs.begin();	
							
				while ( coef < coefs.end() ) {
					if ( *coef < 0 ) *coef *= -1;
					coef++;
				}

				coef = coefs.begin();
				int param1 = *coef;
				coef++;
				int param2 = *coef;
				int	gcd = GCD ( param1, param2 );

				coef++;
				while ( coef < coefs.end() ) {
					gcd = GCD ( gcd, *coef );
					if ( gcd == 1 )	break;
					coef++;
				}

				int constant = writeRef->getCons() - writeRef2->getCons();
				
				if ( constant % gcd == 0 )	return true;

			}

			writeRef2++;
		
		}

		readRef = arrReads.begin();
		while ( readRef < arrReads.end() ) {

			if ( writeRef->getName() == readRef->getName() ) {

				std::vector<int> coefs;
				
				std::vector<IndexVar>::iterator		writeVar = writeRef->getVars()->begin(),
													readVar = readRef->getVars()->begin();

				while ( writeVar < writeRef->getVars()->end() ) {
					coefs.push_back(writeVar->getCoef());
					writeVar++;
				}
				while ( readVar < readRef->getVars()->end() ) {
					coefs.push_back(readVar->getCoef());
					readVar++;
				}

				std::vector<int>::iterator coef = coefs.begin();	
							
				while ( coef < coefs.end() ) {
					if ( *coef < 0 ) *coef *= -1;
					coef++;
				}

				coef = coefs.begin();
				int param1 = *coef;
				coef++;
				int param2 = *coef;
				int	gcd = GCD ( param1, param2 );

				coef++;
				while ( coef < coefs.end() ) {
					gcd = GCD ( gcd, *coef );
					if ( gcd == 1 )	break;
					coef++;
				}

				int constant = writeRef->getCons() - readRef->getCons();
				
				if ( constant % gcd == 0 )	return true;

			}

			readRef++;		
		
		}

		writeRef++;

	}

	return false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										PRINT WRITES
////////////////////////////////////////////////////////////////////////////////////////////////////

void ForLoop::printArrWrites () {

	std::cout << "Array Writes:\n\n";

	std::vector<ArrayRef>::iterator		ref = arrWrites.begin();

	while ( ref < arrWrites.end() ) {	

		ref->print();
		std::cout << "\n";
		ref++;

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										PRINT READS
////////////////////////////////////////////////////////////////////////////////////////////////////

void ForLoop::printArrReads () {

	std::cout << "Array Reads:\n\n";

	std::vector<ArrayRef>::iterator		ref = arrReads.begin();

	while ( ref < arrReads.end() ) {	

		ref->print();
		std::cout << "\n";
		ref++;

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										PRINT ARRAY REFERENCES
////////////////////////////////////////////////////////////////////////////////////////////////////

void ForLoop::printArrayRefs () {

	printArrWrites();
	std::cout << "\n";
	printArrReads();
	std::cout << "\n";

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//										END INCLUDE GUARDS
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif