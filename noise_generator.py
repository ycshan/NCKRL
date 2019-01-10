import random


class GenNoise(object):
    def __init__(self, base_dir, **kwargs):
        self._data_base = base_dir
        self._entities = []
        self._relations = []
        self._train_triple = []
        self._head_constraint = {}
        self._tail_constraint = {}
        self._train_name = kwargs.pop('train', '')
        self._ent2id_name = kwargs.pop('ent2id', '')
        self._rel2id_name = kwargs.pop('rel2id', '')
        self._noise_triple = []

    def load_data_info(self, sp='\t'):
        # triple
        path = self._data_base+self._train_name
        with open(path, 'r') as file:
            lines = file.readlines()
            for line in lines:
                triple = line.strip().split(sp)
                if len(triple) < 3:
                    continue
                self._train_triple.append(tuple(
                    [triple[0], triple[1], triple[2]]))
        # detail
        self._entities = self.read_details(
            self._data_base+self._ent2id_name)
        self._relations = self.read_details(
            self._data_base+self._rel2id_name)

    @staticmethod
    def read_details(path, sp='\t'):
        """
        build id map
        :param path: file path
        :param sp: separation tag
        :return: a list of relations or entities
        """
        detail_lst = []
        with open(path, 'r') as file:
            for line in file.readlines():
                detail = line.strip().split(sp)
                detail_lst.append(detail[0])
        return detail_lst

    def find_type_constraint(self):
        progress = 0
        for tr in self._train_triple:
            key_r = self._relations.index(tr[2])
            # each relation's heads
            if key_r not in self._head_constraint:
                head_entity_set = {self._entities.index(tr[0])}
                self._head_constraint[key_r] = head_entity_set
            else:
                temp = self._head_constraint[key_r]
                temp.add(self._entities.index(tr[0]))
                self._head_constraint[key_r] = temp
            # each relation's tails
            if key_r not in self._tail_constraint:
                tail_entity_set = {self._entities.index(tr[1])}
                self._tail_constraint[key_r] = tail_entity_set
            else:
                temp = self._tail_constraint[key_r]
                temp.add(self._entities.index(tr[1]))
                self._tail_constraint[key_r] = temp
                progress += 1
            print('\r find type constraint: {:.2f}%'.format(
                progress * 100 / len(self._train_triple)), end='')
        print('\n')

        # save constraints to file
        self.write_type_constraint(
            self._data_base+'hc.txt', self._head_constraint)
        self.write_type_constraint(
            self._data_base+'tc.txt', self._tail_constraint)

    @staticmethod
    def write_type_constraint(path, constraint):
        """
        write each relation's head or tail constraints
        to file, if necessary
        :param path: file path
        :param constraint: head or tail
        :return:
        """
        with open(path, 'w') as file:
            for key, val in constraint.items():
                file.write(str(key))
                for entity in val:
                    file.write('\t' + str(entity))
                file.write('\n')

    def generate_noise(self, ratio):
        facts = random.sample(
            self._train_triple, int(len(self._train_triple)*ratio))
        progress = 0
        for tr in facts:
            # replace head
            if random.random() < 0.5:
                if not self.corrupt_entity(tr):
                    continue
            # replace tail
            else:
                if not self.corrupt_entity(tr, is_head=False):
                    continue
            progress += 1
            print('\r generate noise: {:.2f}%'.format(
                progress * 100 / len(facts)), end='')
        # save noisy triplets
        self.write_noise_triple(ratio)

    def corrupt_entity(self, triple, is_head=True):
        """
        corrupt a triple's entity to get a noise
        :param triple: a fact
        :param is_head: place to replace
        :return:
        """
        if is_head:
            entities = self._head_constraint[self._relations.index(triple[2])]
        else:
            entities = self._tail_constraint[self._relations.index(triple[2])]
        if len(entities) < 2:
            return False
        repeat = 0
        while True:
            repeat += 1
            rand_ent = random.sample(entities, 1)[0]
            if is_head:
                tmp_triple = tuple([self._entities[rand_ent], triple[1], triple[2]])
            else:
                tmp_triple = tuple([triple[0], self._entities[rand_ent], triple[2]])
            if tmp_triple not in self._train_triple:
                self._noise_triple.append(tmp_triple)
                break
            elif repeat == 50:
                break
        return True

    def write_noise_triple(self, ratio):
        """
        write noisy triplets to file
        :param ratio: noise ratio
        :return:
        """
        name, ext = name_and_ext(self._train_name)
        noise_name = name+'_'+str(int(ratio*100))+'.'+ext
        path = self._data_base+noise_name
        with open(path, 'w') as file:
            for elem in self._noise_triple:
                file.write(elem[0]+'\t'+elem[1]+'\t'+elem[2]+'\n')


def name_and_ext(f_name):
    fn_ext = f_name.split('.')
    name = fn_ext[:-1]  # file name
    ext = fn_ext[-1:]  # file extension
    return '_'.join(name), ext[0]


if __name__ == '__main__':
    basic_path = './result/WN18/'
    data_info = {'train': 'train.txt',
                 'ent2id': 'entity2id.txt',
                 'rel2id': 'relation2id.txt'
                 }
    noise_ratios = [0.1, 0.2, 0.4]
    generator = GenNoise(basic_path, **data_info)
    generator.load_data_info()
    generator.find_type_constraint()
    for r in noise_ratios:
        generator.generate_noise(r)
