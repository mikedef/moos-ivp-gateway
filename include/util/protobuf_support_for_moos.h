/************************************************************/
/*    NAME: 	Supun Randeni
/*	  E-MAIL:	supun@mit.edu                                           
/*    ORGN: 	MIT                                             
/*    FILE: 	protobuf_support_for_moos.h                                          
/*    DATE: 	2022-03-27                                                
/*    DATE:     2022-11-04 Edited for gateway app by mikedef
/************************************************************/
/*	  Adopted from Goby3's moos_protobuf_helpers.h and simplified
/*	  to pack HydroMAN internal messages                                    
/************************************************************/

#ifndef PROTOBUF_SUPPORT_FOR_MOOS_GATEWAY_H
#define PROTOBUF_SUPPORT_FOR_MOOS_GATEWAY_H

#include <google/protobuf/text_format.h>             // for TextFormat

namespace gateway
{
namespace util
{
	/// \brief Converts the Google Protocol Buffers message `msg` into a suitable (human readable) string `out` for sending via MOOS
	///
	/// \param out pointer to std::string to store serialized result
	/// \param msg Google Protocol buffers message to serialize
	inline bool serialize_for_moos_message(std::string* out, const google::protobuf::Message& msg)
	{
	    google::protobuf::TextFormat::Printer printer;
	    printer.SetSingleLineMode(true);
	    printer.PrintToString(msg, out);
	    return true;
	}

	/// \brief Parses the string `in` to Google Protocol Buffers message `msg`. All errors are written to the goby::util::glogger().
	///
	/// \param in std::string to parse
	/// \param msg Google Protocol buffers message to store result
	inline void parse_from_moos_message(const std::string& in, google::protobuf::Message* msg)
	{
	    google::protobuf::TextFormat::Parser parser;
	    // goby::util::FlexOStreamErrorCollector error_collector(in);
	    // parser.RecordErrorsTo(&error_collector);
	    parser.ParseFromString(in, msg);
	}

}	// gateway namespace
}	// util namespace

#endif
