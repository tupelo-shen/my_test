IOTA Foundation, the Berlin-based nonprofit behind the IOTA cryptocurrency platform, is gearing up to launch an IoT data marketplace to "securely store, sell and access data streams."

A proof of concept is currently open to everyone interested, with an API available that allows users to "contribute sensors and build new data-driven applications." It's up and running in real time on its test network now, according to IOTA's website, with "full end-to-end data verifiability and security offered via masked authenticated messaging within the users' browser."

More than 35 companies are participating in this initiative, which has the goal of exploring how a data marketplace can be built atop a distributed ledger, as well as to uncover potential challenges and applications.

# IOTA's 'tangle'

The secret sauce, or architecture, that differentiates IOTA from other distributed ledgers is its directed acyclic graph (DAG) called "tangle," which doesn't rely on blocks. "Instead, IOTA registers transactions to 'tangle,'" explained Jessica Groopman, industry analyst and founding partner of Kaleido Insights. "Rather than hashing transactions to blocks, transactions are approved by verifying two other transactions."

To add a transaction, for example, "the protocol selects two other transactions at random, their association keys and balances are validated then bundled together with the original transaction and proof of work it calls a 'Curl.' This bundle is sent out to the network to be verified by the next generation of transactions," she added.

The approach of "distributing the proof of work -- instead of relegating it to miners -- means that to participate in the network, you must contribute to verifying others' transactions," Groopman said. "There's also no need to transfer tokens to verify transactions. This is a fundamentally different architecture than private or public blockchain configurations and contributes to IOTA's distinction of 'not charging transaction fees.' In effect, the cost of participating is to validate others' transactions. The absence of transaction fees means IOTA is better suited for high-frequency microtransactions, such as IoT applications."

That is, if it really turns out to be completely free of transaction fees. As MIT's Digital Currency Initiative recently pointed out, this aspect of tangle remains to be proven.

# IOTA Foundation's security concerns

> Virtually all IoT data should be secured across the stack -- and, most of the time, this means tamperproof.

Is it necessary for all shared IoT data to be tamperproof? "Virtually all IoT data should be secured across the stack -- and, most of the time, this means tamperproof," Groopman said.

IOTA's tangle is "best suited for IoT applications involving high-volume and high-frequency data -- from smart cities, energy, oil and gas, aerospace, and automotive," she pointed out. "But IOTA is far from a proven technology. In fact, some question its cryptography and the fact that, in its early days, it still runs off what it calls the 'coordinator.' This is a closed source operating system that protects the network when transaction volumes are low, which it intends to remove once the network grows. Many argue the coordinator is just another form of centralization."

Beyond the question of whether or not tangle is really decentralized, IOTA's tangle whitepaper itself raised security concerns.

"I haven't been able to convince myself of its security," said Andrew Miller, assistant professor of Electrical and Computer Engineering at the University of Illinois at Urbana-Champaign and co-director of IC3 (Initiative for Cryptocurrencies and Contracts). "For a distributed consensus protocol, the engineering standard is a security proof of liveness and safety in a clearly articulated network model. Instead of this, the whitepaper only describes defenses against a few specific classes of attacks. This analysis doesn't rule out other attacks not yet thought of."

Miller noted that the bitcoin whitepaper also does not contain a proof, but such analysis came later. "Regardless, it's an ongoing effort to improve the analysis in more and more realistic models -- the 51% honest assumption doesn't address incentive-based attacks like bribery, and we don't have a good defense against bribery attacks in any cryptocurrency," he said.

Miller, and many others, would like to see a thorough security analysis from independent researchers. "As [Christian] Cachin and [Marko] Vukulic mentioned in their "Blockchain Consensus Protocols in the Wild" keynote talk at DISC 2017 (International Symposium on Distributed Computing), IOTA's whitepaper and documentation claim that the tangle hash-DAG ensures a similar level of consistency as other permissionless blockchain systems. No publicly reviewed, formal analysis is available, however. Without any independent assessment of the protocol's properties, it remains unclear how strictly the tangle emulates a notion of consensus among the nodes,'" he said.

The IOTA Foundation itself is calling for more research on the matter.

"Does the model truly work? It's definitely still an open question," said John Licciardello, managing director of the ecosystem development at IOTA Foundation. "I don't put on rose-colored glasses and say that. Mathematicians who know this stuff way better than me say some of the aspects are just too difficult to prove right now, but the intuition makes sense for them." He acknowledged that a lot more research is necessary.

# Timeline for the IoT data marketplace

Are companies ready to make full use of distributed ledgers and open markets of IoT data?

"Short answer: no. These are extremely early days for IoT and blockchain convergence," Groopman said. "Not only are companies hesitant to place high-volume, sometimes mission-critical transaction data on such a nascent technology architecture -- never mind one run by a startup -- companies are still grappling with IoT data management, analysis and security in current architectures. Adding the extra cultural barrier of data sharing to the mix makes many companies even more hesitant."

Distributed ledger technologies, in general, "present a number of promising solutions to current IoT challenges -- data sharing is a big one -- but adoption suffers from the classic chicken and egg conundrum these days. Without proven industry examples, companies are hesitant to invest beyond pilots. But without greater investment, projects stay in pilot and industry developments creep along," she noted.

Will the IoT data marketplace concept eventually become ubiquitous? "There's a huge need for data sharing within the IoT space, and other technologies as well -- artificial intelligence, augmented reality and virtual reality, and autonomous vehicles," Groopman said.

But "the imperative, never mind strategy, of sharing still is counterintuitive for many businesses because it runs against highly ingrained 'proprietary' ways of thinking," she continued. "Sharing data isn't like sharing a secret recipe or competitive advantage. In the digital world, it's often essential for better algorithms, stronger security, interoperability and service-based business models. Beyond those cultural hurdles, companies need a trusted mechanism for securing, permissioning, authenticating and tracking shared data."

This is where distributed ledger technologies offer promise, Groopman added, noting that "we're in the very early days. Sharing only works when trust is present -- if interpersonal or organizational trust is impossible, then companies need architectural trust."

# Trust and behavior matter

It's important to note that IOTA is also currently being red flagged for some of its recent behavior -- it responded with hostility to a group of researchers from MIT and Boston University who found a "gaping security hole" within its software and then expressed concerns about whether or not tangle actually lives up to all of its claims.

"I'm aware of the 'sketchy' sentiments expressed toward IOTA on cryptocurrency social media, but I try not to put too much faith in that," Miller said. "However, I found its hostile interactions toward researchers to be completely out of line. Neha Narula, Ethan Heilman, Tadge Dryja and Madars Virza are all world-class scientific contributors to cryptocurrency. Pick fights with scientists at your peril!"

The researchers' "concerns about IOTA's collision-resistant hash functions have been addressed, but the approach to development this revealed -- using ad hoc, ternary crypto for a poorly justified reason, reacting poorly to criticism -- are absolutely red flags. Of course, these are just signals and the protocol could still turn out to be secure -- I haven't drawn a conclusion yet either way," Miller added.

As Miller mentioned, the IOTA Foundation responded to the MIT researchers' concerns in a four-part blog, which IOTA claims "touches on the clarity of its communications, security issues concerning the coordinator, the rationale of the Curl and more."

And in a recent show of support, Germany-based Robert Bosch Venture Capital GmbH (RBVC) purchased "a significant amount" of IOTA tokens. As Ingo Rameshol, managing director at RBVC, put it, "Distributed ledger technology will play an important role in the industry of tomorrow."

IOTA hopes to have go-to-market technology ready by the end of 2018. But the bottom line is that any IoT data marketplace will need to demonstrate that it's secure before companies can really begin to take advantage of it.