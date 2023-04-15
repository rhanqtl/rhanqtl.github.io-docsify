#!/usr/bin/env ruby

class MyBase
  def say_something
    puts "#{self.class}.say_something"
  end
end

module Speaker
  def say_something
    puts "Speaker::say_something"
  end
end

class MyDerived < MyBase
  include Speaker
end

o = MyDerived.new

o.say_something

module Walker
  def say_something
    puts "Walking"
  end
end

class MyDerived
  include Walker
end

o.say_something
