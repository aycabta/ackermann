require 'continuation'
require 'benchmark'

def ack(m, n)
  if m == 0
    n + 1
  elsif n == 0
    ack(m - 1, 1)
  else
    ack(m - 1, ack(m, n - 1))
  end
end

def cc_ack(m, n)
  stack = Array.new
  cont = nil
  m, n = callcc do |cc|
    cont = cc
    [m, n]
  end
  if m == 0
    if stack.size == 0
      n + 1
    else
      node = stack.pop
      cont.call([node[:m] - 1, n + 1])
    end
  elsif n == 0
    cont.call([m - 1, 1])
  else
    stack << {:m => m, :n => n}
    cont.call([m, n - 1])
  end
end

def cc_general_ack(m, n)
  stack = Array.new
  result = 0
  start = nil
  m, n = callcc do |cc|
    start = cc
    [m, n]
  end
  if m == 0
    if stack.size == 0
      return n + 1
    else
      node = stack.pop
      result = n + 1
      node[:cc].call(node[:m], node[:n])
    end
  elsif n == 0
    start.call(m - 1, 1)
  else
    cont = nil
    m, n = callcc do |cc|
      cont = cc
      stack << {:m => m, :n => n, :cc => cont}
      start.call(m, n - 1)
    end
    start.call(m - 1, result)
  end
end

class Module
  NoStacklessMethodSuffix = '__no_stackless'
  def stackless_method(*names)
    allow_stack = Numeric===names.last ? names.pop : 200
    names.each do |name|
      raise NameError, "method `#{name}' for class `#{self}'" \
        " is already stackless" if stackless_method? name
      stk_name = (name.to_s + ::Module::NoStacklessMethodSuffix).intern
      alias_method stk_name, name
      define_method(name) do |*args, &blk|
        if caller(allow_stack).nil?
          send stk_name, *args, &blk
        else
          Fiber.new(&method(stk_name)).resume(*args, &blk)
        end
      end
    end
  end

  def stackless_method?(name)
    method_defined?((name.to_s + ::Module::NoStacklessMethodSuffix).intern)
  end

  def remove_stackless(*names)
    names.each do |name|
      raise NameError, "unknown stackless method `#{name}' " \
        "for class `#{self}'" unless stackless_method? name
      stk_name = (name.to_s + ::Module::NoStacklessMethodSuffix).intern
      alias_method name, stk_name
      remove_method stk_name
    end
  end
end

module Math
  def self.stackless_ack(m, n)
    if m == 0
      n + 1
    elsif n == 0
      self.stackless_ack(m - 1, 1)
    else
      self.stackless_ack(m - 1, self.stackless_ack(m, n - 1))
    end
  end
end

def stackless_ack_call(m, n)
  class << Math
    stackless_method :stackless_ack
  end
  Math.stackless_ack(m, n)
end


def run_ack(func, m, n)
  begin
    puts "start: #{func}(#{m}, #{n})\n"
    result = nil
    t = Benchmark.measure do
      result = Object.send(func, m, n)
    end
    puts "result: #{result}\n"
    puts t.format("%.8r") + "\n"
  rescue Exception => e
    puts "ERROR\n"
    puts "#{e.message}\n"
    e.backtrace.each do |line|
      puts "#{line}\n"
    end
  end
end

m_init = 4
n_init = 1

run_ack(:ack, m_init, n_init)
run_ack(:cc_general_ack, m_init, n_init)
run_ack(:cc_ack, m_init, n_init)
run_ack(:stackless_ack_call, m_init, n_init)

