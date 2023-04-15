#!/usr/bin/env ruby

code = <<EOF
puts 1+1
EOF

puts 1+1

puts RubyVM::InstructionSequence.compile(code).disasm

class Integer
  alias_method :old_add, :+
  def +(rhs)
    puts "#{self} + #{rhs}"
    self.old_add(rhs)
  end
end

puts 1+1

puts RubyVM::InstructionSequence.compile(code).disasm
