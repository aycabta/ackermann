import java.math.BigInteger;
import java.util.ArrayList;

interface Ackermann {
	abstract BigInteger Ack(BigInteger m, BigInteger n);
}

class AckermannNormal implements Ackermann {
	public BigInteger Ack(BigInteger m, BigInteger n) {
		if (m.equals(BigInteger.ZERO)) {
			return n.add(BigInteger.ONE);
		} else if (n.equals(BigInteger.ZERO)) {
			return Ack(m.subtract(BigInteger.ONE), BigInteger.ONE);
		} else {
			return Ack(m.subtract(BigInteger.ONE), Ack(m, n.subtract(BigInteger.ONE)));
		}
	}
}

class AckermannWhile implements Ackermann {
	private ArrayList<BigInteger> stack = new ArrayList<BigInteger>();

	public BigInteger Ack(BigInteger mInput, BigInteger nInput) {
		BigInteger m;
		BigInteger n;

		m = new BigInteger(mInput.toByteArray());
		n = new BigInteger(nInput.toByteArray());
		while (true) {
			if (m.equals(BigInteger.ZERO)) {
				if (stack.size() == 0) {
					return n.add(BigInteger.ONE);
				} else {
					m = stack.remove(stack.size() - 1).subtract(BigInteger.ONE);
					n = n.add(BigInteger.ONE);
				}
			} else if (n.equals(BigInteger.ZERO)) {
				m = m.subtract(BigInteger.ONE);
				n = BigInteger.ONE;
			} else {
				stack.add(m);
				n = n.subtract(BigInteger.ONE);
			}
		}
	}
}

class AckermannRunner {
	static void Run(String className, BigInteger m, BigInteger n) {
		long start;
		long end;
		BigInteger result;
		try {
			Class clazz = Class.forName(className);
			if (clazz == null) {
				System.out.println("ERROR");
				System.out.println("Class \"" + className + "\" not found");
				return;
			}
			System.out.println("start: (new " + className + "()).Ack(" + m.toString(10) + ", " + n.toString(10) + ")");
			Ackermann ack = (Ackermann)clazz.newInstance();
			start = System.nanoTime();
			result = ack.Ack(m, n);
			end = System.nanoTime();
			System.out.println("result: " + result.toString(10));
			System.out.println(String.format("%.8f", (double)(end - start) / 1000000000));
		} catch (Throwable e) {
			System.out.println("ERROR");
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		BigInteger m = new BigInteger("4");
		BigInteger n = new BigInteger("1");
		Run("AckermannNormal", m, n);
		Run("AckermannWhile", m, n);
	}
}

