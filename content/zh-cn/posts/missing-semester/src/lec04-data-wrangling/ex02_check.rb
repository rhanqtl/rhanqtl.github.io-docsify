#!/usr/bin/env ruby

words = File.open("/usr/share/dict/words").
    readlines.
    map(&:chomp).
    map(&:downcase)

three_as_words = words.filter { |w| w.count('a') >= 3 and not w.end_with? "'s" }

suffixes = three_as_words.map { |w| w[-2,2] }

suffixes_stats = suffixes.sort.
    inject(Hash.new(0)) { |h, e| h[e] += 1; h }.
    map { |s, c| [s, c] }.
    sort_by { |t| -t[1] }

puts suffixes_stats.map { |t| "#{t[1]} #{t[0]}" }.slice(0, 3)
