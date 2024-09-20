using System.Net.Sockets;
using System.Text;

class Program
{
    static readonly int PORT = 65535;
    static readonly string IP = "";

    static void Main()
    {
        UdpClient udpClient = new(PORT);

        try
        {
            udpClient.Connect(IP, PORT);

            var bytes = Encoding.ASCII.GetBytes("Hello World!");

            var sentBytes = udpClient.Send(bytes, bytes.Length);

            Console.WriteLine($"Sent {sentBytes} bytes to {IP}");
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
        finally
        {
            udpClient.Close();
        }
    }
}