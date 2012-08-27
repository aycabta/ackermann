require 'continuation'
require 'benchmark'
require 'active_support'

include ActiveSupport::Memoizable

def ack(m, n)
  if m == 0
    n + 1
  elsif n == 0
    ack(m - 1, 1)
  else
    ack(m - 1, ack(m, n - 1))
  end
end
memoize :ack

def cc_ack(m, n)
  stack = [nil]
  sp = 0
  cont = nil
  args = callcc do |cc|
    cont = cc
    [m, n]
  end
  stack[sp] = {:m => args[0], :n => args[1]}
  if stack[sp][:m] == 0
    if sp == 0
      stack[sp][:n] + 1
    else
      node = stack.pop
      sp = sp - 1
      cont.call([stack[sp][:m] - 1, node[:n] + 1])
    end
  elsif stack[sp][:n] == 0
    cont.call [stack[sp][:m] - 1, 1]
  else
    stack << nil
    sp = sp + 1
    cont.call([stack[sp - 1][:m], stack[sp - 1][:n] - 1])
  end
end

def cc_general_ack(m, n)
  stack = Array.new
  sp = 0
  result = 0
  start = nil
  m, n = callcc do |cc|
    start = cc
    [m, n]
  end
  if m == 0
    if sp == 0
      return n + 1
    else
      sp = sp - 1
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
      sp = sp + 1
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
    p "start: " + func.to_s + "(" + m.to_s + ", " + n.to_s + ")"
    result = nil
    t = Benchmark.measure do
      result = Object.send(func, m, n)
    end
    p "result: " + result.to_s
    puts t.to_s
  rescue Exception => e
    p e.message
    e.backtrace.each do |line|
      p line
    end
  end
end

m_init = 4
n_init = 2

run_ack(:ack, m_init, n_init)
run_ack(:cc_general_ack, m_init, n_init)
run_ack(:cc_ack, m_init, n_init)
run_ack(:stackless_ack_call, m_init, n_init)

