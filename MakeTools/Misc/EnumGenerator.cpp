/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdafx.h"

#include "EnumGenerator.h"
#include <iomanip>

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/current_function.hpp>
#include <string>
#include <ctime>

static int nbSpace = 200;
static char Tag [] = "    ";


void writeHeader(std::ofstream& file)
{
	
	file<<"typedef std::string String_t;\n\n"; 
	file<<"#define CASE_ENUM_TO_STRING(str, value)"<<std::setw(nbSpace)<<"\\\n"<<Tag<<Tag<<"case value:"<<std::setw(nbSpace)<<"\\\n"<<Tag<<Tag<<Tag<<"str = #value;"<<std::setw(nbSpace)<<"\\\n"<<Tag<<Tag<<Tag<<"break;\n\n\n";
}

void writeEnum(std::ofstream& file, const int nbParam)
{
	int ii;

/////////////////////////////
//le define
/////////////////////////////
	file<<"#define LM_ENUM_"<<nbParam<<"(name, ";
	for (ii=0; ii<nbParam-1; ++ii)
		file<<"param_"<<ii<<", ";
	file<<"param_"<<ii<<')';
	file<<std::setw(nbSpace)<<"\\\n";

/////////////////////////////
//l'enum
/////////////////////////////
	file<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"enum name {";
	for (ii=0; ii<nbParam-1; ++ii)
		file<<"param_"<<ii<<", ";
	file<<"param_"<<ii;
	file<<"};";

/////////////////////////////
//toString
/////////////////////////////
	file<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"String_t toString(const name e)"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"{"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"String_t str;"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"switch(e)"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"{"<<std::setw(nbSpace)<<"\\\n";
	for (ii=0; ii<nbParam; ++ii)
	{
		file<<Tag<<Tag<<Tag<<"CASE_ENUM_TO_STRING(str, "<<"param_"<<ii<<");"<<std::setw(nbSpace)<<"\\\n";
	}
	file<<Tag<<Tag<<"}"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"return str;"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"}"<<std::setw(nbSpace)<<"\\\n";

/////////////////////////////
//isValid
/////////////////////////////
	file<<Tag<<"bool isValid(const name e)"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"{"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"bool result = false;"<<std::setw(nbSpace)<<"\\\n";
    file<<Tag<<Tag<<"switch(e)"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"{"<<std::setw(nbSpace)<<"\\\n";
	for (ii=0; ii<nbParam; ++ii)
	{
		file<<Tag<<Tag<<Tag<<"case "<<"param_"<<ii<<":"<<std::setw(nbSpace)<<"\\\n";
    }
	file<<Tag<<Tag<<Tag<<Tag<<"result = true;"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<Tag<<"}"<<std::setw(nbSpace)<<std::setw(nbSpace)<<"\\\n";
    file<<Tag<<Tag<<"return result;"<<std::setw(nbSpace)<<"\\\n";
	file<<Tag<<"}"<<std::setw(nbSpace)<<"\\\n";
/////////////////////////////
//Count
/////////////////////////////
	file<<Tag<<"const int32 "<<"name"<<"##_COUNT"<<" = "<<nbParam<<";";
/////////////////////////////
//fin
/////////////////////////////
	file<<"\n\n\n";
}

void GenerateEnum(std::string fileName)
{
	const int nbMaxEnum = 50;

	std::ofstream file ( fileName.c_str(), std::ios_base::beg );

	writeHeader(file);

	for (int idEnum=1; idEnum<nbMaxEnum; ++idEnum)
		writeEnum(file, idEnum);
}