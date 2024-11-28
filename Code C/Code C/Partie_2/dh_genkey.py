import threading
import queue
import time
import sys
import argparse

if len(sys.argv) != 5:
    print("Usage: python3 dh_genkey.py -i <file_name> -o <file_name>")
    sys.exit(1)
parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input",required=True,help="Input file name")
parser.add_argument("-o", "--output",required=True,help="Output file name")
args = parser.parse_args()
input_file = args.input
output_file = args.output


def getPandG(file_name):
    with open(file_name, "r", encoding="utf-8") as fichier:
        return fichier.readlines()

def puissance_mod_n(base, exp, mod):
    result = 1
    base = base % mod
    while exp > 0:
        if exp % 2 == 1:  # Si l'exposant est impair
            result = (result * base) % mod
        exp = exp >> 1  # Divise l'exposant par 2
        base = (base * base) % mod
    return result


class Exchange:
    def __init__(self):
        print("START")
        lignes_fichier = getPandG(input_file)
        self.p = int(lignes_fichier[0])
        self.g = int(lignes_fichier[1])
        self.queue = queue.Queue()
        self.stop_event = threading.Event()  # Pour indiquer quand arrêter Eve
        # Création des sémaphores
        self.semaphore_alice = threading.Semaphore(0)  # Alice attend Bob
        self.semaphore_bob = threading.Semaphore(0)    # Bob attend Alice
            
    def alice(self):
        a = 6
        A = puissance_mod_n(self.g, a, self.p)
        print("Alice envoie A sur le réseau:", A)
        
        # Envoyer A à Bob et débloquer son sémaphore
        self.queue.put(A)
        time.sleep(1)
        self.semaphore_bob.release()  # Permet à Bob de récupérer A

        # Attendre que Bob envoie B
        self.semaphore_alice.acquire()  # Alice attend que Bob ait envoyé B

        # Récupérer B et calculer la clé secrète
        B = self.queue.get()
        K = puissance_mod_n(B, a, self.p)
        print("Alice connaît p = ", self.p, ", g = ", self.g, ", a = ", a, ", A = ", A, ", B = ", B)
        print("Clé secrète d'Alice:", K)

    def bob(self):
        b = 15
        # Attendre qu'Alice envoie A
        self.semaphore_bob.acquire()  # Bob attend que Alice ait envoyé A

        # Récupérer A et envoyer B
        A = self.queue.get()
        B = puissance_mod_n(self.g, b, self.p)
        print("Bob envoie B sur le réseau:", B)
        
        # Envoyer B à Alice et débloquer son sémaphore
        self.queue.put(B)
        time.sleep(1)
        self.semaphore_alice.release()  # Permet à Alice de récupérer B

        # Calculer la clé secrète
        K = puissance_mod_n(A, b, self.p)
        print("Bob connaît p = ", self.p, ", g = ", self.g, ", b = ", b, ", A = ", A, ", B = ", B)
        print("Clé secrète de Bob:", K)
        # Signaler à Eve que l'échange est terminé
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
    


# Création de l'échange Diffie-Hellman
exchange = Exchange()

# Création des threads pour Alice et Bob
alice_thread = threading.Thread(target=exchange.alice)
bob_thread = threading.Thread(target=exchange.bob)
eve_thread = threading.Thread(target=exchange.eve)

# Lancer les threads
alice_thread.start()
bob_thread.start()
eve_thread.start()

# Attendre que les threads terminent
alice_thread.join()
bob_thread.join()
eve_thread.join()
