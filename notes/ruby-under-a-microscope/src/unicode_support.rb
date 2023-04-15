#!/usr/bin/env ruby

s = "நிกำषि"

p s.length

puts "=== Each Character ==="
puts s.each_char.count

puts "=== Each Byte ==="
puts s.each_byte.count

puts "=== Each Grapheme Cluster ==="
puts s.each_grapheme_cluster.count
