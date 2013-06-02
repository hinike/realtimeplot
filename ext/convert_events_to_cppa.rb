@classes = {}

@status = ""

@current_class =""

event_h = File.open( "include/realtimeplot/events.h" )

# Parsing file
event_h.lines.each do |line|
	match_class = line.match( /class (.*)\s+: public Event/ )
	if match_class
		#if @status != "handled class" && @current_class != ""
		#	puts "Failed parsing class: " + @current_class
		#end
		@status = "inside class"
		@current_class = match_class[1]
		#puts @status + ": " + @current_class
	end
	if @status == "inside class"
		# Look for constructor
		match_constr = line.match( /#{@current_class}.*\((.*).*\)/ );
		if match_constr
			@status = "handled class"
			args = match_constr[1].strip.split(",")
			@classes[@current_class] = {"types"=>[],"names"=>[]}
			args.each do |ar|
				vartypes = ar.strip.split( " " )
				if vartypes.length > 2
					vartypes = [vartypes[0..(vartypes.length-2)].join(" "), vartypes.last]
				end
				type = vartypes[0]
				name = vartypes[1]
				if type == "boost::shared_ptr<EventHandler>" || type =~ /boost::shared_ptr/
					# Can't convert these yet
					@classes.delete( @current_class )
					@status = "inside class"
					break
				end
				@classes[@current_class]["types"].push( type )
				@classes[@current_class]["names"].push( name )
			end
		end
	end
end

event_h.close


# generate c++ code for node >> pEvent
code = '// This file is automatically generated
#include "realtimeplot/yaml.h"

using namespace realtimeplot;

void operator >> (const YAML::Node& node, 
		boost::shared_ptr<realtimeplot::Event> &pEvent) {
	std::string name;
	node["name"] >> name;
	'

@classes.each do |name, vars|
	name = name.sub(/Event$/, "")
	code += "cppa::on( "
	code += "cppa::atom(\"#{name.downcase}\"), cppa::arg_match) >>  [&](\n\t"
	arg_array = []
	vars["types"].each_with_index do |t, i|
		n = vars["names"][i]
		arg_array.push( "const #{t} &#{n}" )
	end
	code += arg_array.join( ", " ) + ") {\n" + "\t\t...\n" + "},\n"
end

code += '{
			std::cout << "Yaml event unsupported" << std::endl;
			pEvent.reset(new Event());
	}
}'

puts code
