

extern "C" {
	void setup() {
		Bigint pos = Bigint::getHoweverManyDigits(0);
        BigObjects::globalCamera.position.x = BigObjects::globalCamera.position.x + pos;
        BigObjects::globalCamera.position.z = BigObjects::globalCamera.position.z - Bigint(10);
        BigObjects::globalCamera.rotation.y = BigObjects::globalCamera.rotation.y + 180;

        bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        bigObjects[bigObjects.size()-1]->position.x += pos;
        bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        bigObjects[bigObjects.size()-1]->position.x -= Bigint(10);
        bigObjects[bigObjects.size()-1]->position.x += pos;
        bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[0].get()));
        bigObjects[bigObjects.size()-1]->position.x += Bigint(10);
        bigObjects[bigObjects.size()-1]->position.x += pos;

        bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[2].get()));
        bigObjects[bigObjects.size()-1]->position.x -= Bigint("150000000000");
        bigObjects[bigObjects.size()-1]->position.x += pos;
        bigObjects.push_back(std::make_unique<BigObjects::Singularity>(meshes[1].get()));
        bigObjects[bigObjects.size()-1]->position.y -= Bigint("6378000");
        bigObjects[bigObjects.size()-1]->position.x += pos;

        int size = 11;
        for (int i = 0; i < size; i++) {
            std::unique_ptr<BigObjects::Singularity> lineObject = std::make_unique<BigObjects::Singularity>(meshes[0].get());
            lineObject->position.z = lineObject->position.z + Bigint(10 * (i+1));
            lineObject->position.x = lineObject->position.x + pos;
            bigObjects.push_back(std::move(lineObject));
        }
	}
}