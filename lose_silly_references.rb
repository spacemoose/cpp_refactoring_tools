#!/usr/bin/ruby
exclusion_list = Regexp.union("std::string identifySelf(", "Int setSort_no(", "void setSyscode_id(", "void setSysowner_id(", "Int setIdValueByFieldname(", "Int setIdValueByFieldnameBase(", "Int setBitValueByFieldnameBase(")

ARGV.each do|fname|
  tempname = "foo.txt"
  lines = File.readlines(fname);
  ofile = File.open(tempname, 'w')
  lines.each do |l|
    if exclusion_list.match(l)
      next until l.match('\)')
    else
      l = l.gsub('const ULongInt& rujIn', 'const ULongInt ujIn')
      l = l.gsub(/(?<!& )rujIn/, 'ujIn')
      l = l.gsub('const bool& rbIn', 'const bool bIn')
      l = l.gsub(/\brbIn/, 'bIn')
      ofile.puts l
    end
  end
  File.rename(tempname, fname)
end
