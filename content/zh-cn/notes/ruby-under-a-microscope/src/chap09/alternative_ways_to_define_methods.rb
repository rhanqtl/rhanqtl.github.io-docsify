#!/usr/bin/env ruby

puts "*** Defining Class Methods ***"

code_1 = <<END
class A
  def self.m ; end
end
END

puts RubyVM::InstructionSequence.compile(code_1).disasm

puts "\n*** Defining Singleton Methods ***"

code_2 = <<END
class A
end

a_obj = A.new

def a_obj.m ; end
END

puts RubyVM::InstructionSequence.compile(code_2).disasm
