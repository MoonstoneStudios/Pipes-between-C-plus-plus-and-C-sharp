using System.IO.Pipes;

namespace PipesSimpleDotNet
{
    internal class Program
    {
        // Credit: https://dev.to/gabbersepp/ipc-between-c-and-c-by-using-named-pipes-4em9
        static void Main(string[] args)
        {
            // The pipe.
            using NamedPipeServerStream server = new NamedPipeServerStream("test_pipe_yay", PipeDirection.InOut, 
                    1, PipeTransmissionMode.Byte);

            Console.WriteLine("====C#====");
            Console.WriteLine("Waiting for connection...");

            // Halts code until connection is made.
            server.WaitForConnection();
            StreamReader reader = new StreamReader(server);
            StreamWriter writer = new StreamWriter(server);

            while (true)
            {
                Console.WriteLine("send<< Angle Request.");
                writer.Write("Angle request.");
                writer.Write((char)0);      // End with a null character to tell C++ string is over.
                writer.Flush();             // Send the data
                server.WaitForPipeDrain();  // Wait for C++ to read the data

                // Get the response from C++
                string? angle = reader.ReadLine();
                // Response must end in \r\n, if not, return.
                if (angle is null) return;

                Console.WriteLine($"reciv<< {angle}");

                if (angle.StartsWith("Angle: "))
                {
                    // Get the angle value.
                    int angleValue = Convert.ToInt32(angle.Substring("Angle: ".Length));
                    Console.WriteLine($" - - Mems angle is now {angleValue} degrees");
                    Console.WriteLine($"send<< Set to {angleValue}");

                    // Send confirmation.
                    writer.Write($"Set to {angleValue}");
                    writer.Write((char)0);
                    writer.Flush();
                    server.WaitForPipeDrain();

                    Thread.Sleep(500);
                }
                // Unknown response, return.
                else return;
            }
        }
    }
}
