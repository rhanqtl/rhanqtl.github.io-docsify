require 'benchmark'

ITERATIONS = 10000000

Benchmark.bm do |bench|
  bench.report("test") do
    ITERATIONS.times do
      str = "This string is not shared" + " and so can be modified faster."
      str2 = "But this string is shared" + " so Ruby will need to copy it before writing to it."
      str3 = str2.dup
      #str3[3] = 'x'
      str[3] = 'x'
    end
  end
end
