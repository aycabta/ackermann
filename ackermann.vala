using GLib;

public class Test.HelloObject : GLib.Object {

	public static int ack(int m, int n)
	{
		if (m == 0) return n + 1;
		if (n == 0) return ack(m - 1, 1);
		return ack(m - 1, ack(m, n - 1));
	}

	public static int main(string[] args)
	{
		stdout.printf("%d\n", ack(args[1].to_int(), args[2].to_int()));
		return 0;
	}
}
