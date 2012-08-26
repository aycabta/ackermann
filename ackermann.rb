require 'continuation'
require 'benchmark'
require 'active_support'
require './stackless'

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

def lazy_ack(m, n)
  if (m_entity = m.call) == 0
    n.call + 1
  elsif (n_entity = n.call) == 0
    lazy_ack(lambda{m_entity - 1}, lambda{1})
  else
    lazy_ack(lambda{m_entity - 1}, lambda{lazy_ack(lambda{m_entity}, lambda{n_entity - 1})})
  end
end
memoize :lazy_ack

def cc_ack(m, n)
  max_sp = 0
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
    if sp > max_sp
      max_sp = sp
      p 'sp record: ' + max_sp.to_s
    end
    cont.call([stack[sp - 1][:m], stack[sp - 1][:n] - 1])
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
n_init = 1

#run_ack(:ack, m_init, n_init)
#run_ack(:cc_ack, m_init, n_init)
run_ack(:stackless_ack_call, m_init, n_init)

