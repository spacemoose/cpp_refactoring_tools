#!/usr/bin/ruby

ARGV.each do|fname|
  lines = File.readlines(fname);
  lines.unshift lines.delete_at(lines.index{ |l| l =~ /^\s*#pragma\s*once/})
  lines.each do |l|
    puts(l)
  end
end


#pragma once
