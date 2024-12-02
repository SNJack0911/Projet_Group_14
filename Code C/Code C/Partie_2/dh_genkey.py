import threading
import queue
import time
import sys
import argparse
import random
random.seed(0)
# Argument parsing for input and output file
if len(sys.argv) != 5:
    print("Usage: python3 dh_genkey.py -i <file_name> -o <file_name>")
    sys.exit(1)

parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input", required=True, help="Input file name")
parser.add_argument("-o", "--output", required=True, help="Output file name")
args = parser.parse_args()
input_file = args.input
output_file = args.output


# Utility function to read p and g from a file
def getPandG(file_name):
    with open(file_name, "r", encoding="utf-8") as file:
        return file.readlines()


# Modular exponentiation function
def puissance_mod_n(base, exp, mod):
    result = 1
    base = base % mod
    while exp > 0:
        if exp % 2 == 1:
            result = (result * base) % mod
        exp = exp >> 1
        base = (base * base) % mod
    return result


# Diffie-Hellman Exchange Class
class Exchange:
    def __init__(self):
        print("START")
        lines = getPandG(input_file)
        self.p = int(lines[0])  # Prime number
        self.g = int(lines[1])  # Generator
        self.queue = queue.Queue()
        self.stop_event = threading.Event()  # To signal Eve when to stop

        # Semaphores for synchronization
        self.semaphore_alice = threading.Semaphore(0)  # Alice waits for Bob
        self.semaphore_bob = threading.Semaphore(0)    # Bob waits for Alice

    def alice(self):
        a = random.randint(1, 100)
        A = puissance_mod_n(self.g, a, self.p)
        print("Alice envoie A sur le réseau:", A)

        # Send A to Bob and unblock his semaphore
        self.queue.put(A)
        time.sleep(1)
        self.semaphore_bob.release()

        # Wait for Bob to send B
        self.semaphore_alice.acquire()

        # Retrieve B and calculate the secret key
        B = self.queue.get()
        K = puissance_mod_n(B, a, self.p)
        print(f"Alice connaît: p = {self.p}, g = {self.g}, a = {a}, A = {A}, B = {B}")
        print("Clé secrète d'Alice:", K)

    def bob(self):
        b = random.randint(1, 100)
        self.semaphore_bob.acquire()  # Wait for Alice to send A

        # Retrieve A and calculate B
        A = self.queue.get()
        B = puissance_mod_n(self.g, b, self.p)
        print("Bob envoie B sur le réseau:", B)

        # Send B to Alice and unblock her semaphore
        self.queue.put(B)
        time.sleep(5)
        self.semaphore_alice.release()

        # Calculate the secret key
        K = puissance_mod_n(A, b, self.p)
        print(f"Bob connaît: p = {self.p}, g = {self.g}, b = {b}, A = {A}, B = {B}")
        print("Clé secrète de Bob:", K)

        # Signal to Eve that the exchange is done
        self.stop_event.set()

    def eve(self):
        print("Eve commence à écouter la file d'attente...")
        listes_messages = []
        while not self.stop_event.is_set() or not self.queue.empty():
            try:
                message = self.queue.queue[0]  # Attend un message
                print(self.queue.queue)
                listes_messages.append(message)
                print(f"Eve intercepte un message {message}")
                time.sleep(2)
            except queue.Empty:
                continue  # Réessayer si la file est vide
        print("Eve connaît p = ", self.p, ", g = ", self.g, ", A = ", listes_messages[0], ", B = ", listes_messages[1])
        print("Clé secrète: ...")
        print("Fin de l'écoute")


# Create the Diffie-Hellman exchange instance
exchange = Exchange()

# Create threads for Alice, Bob, and Eve
alice_thread = threading.Thread(target=exchange.alice)
bob_thread = threading.Thread(target=exchange.bob)
eve_thread = threading.Thread(target=exchange.eve)

# Start the threads
alice_thread.start()
bob_thread.start()
eve_thread.start()

# Wait for threads to complete
alice_thread.join()
bob_thread.join()
eve_thread.join()
